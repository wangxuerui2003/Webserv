/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 18:00:03 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/02 17:01:44 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionHandler.hpp"
#include "webserv.hpp"


ConnectionBuffer::ConnectionBuffer() {
	requestString = "";
	request = NULL;
	waitingForMsgBody = false;
	isChunkedRequest = false;
	readyToResponse = false;
}

ConnectionBuffer::~ConnectionBuffer() {
	delete request;
}


/**
 * Generate listening sockets for each Server object
 * Track the host:port pairs that are in the servers,
 * for port that has a bind on host 0.0.0.0, bind once for 0.0.0.0 only.
 * For port that has multiple hosts to bind, bind only the unique hosts, duplicates are ignored.
*/
ConnectionHandler::ConnectionHandler(const std::vector<Server>& servers) : _servers(servers) {
	std::map<std::string, std::set<std::string> > hostsToBind;
	// use a set to maintain unique hosts for one port

	for (std::vector<Server>::const_iterator serverConfig = _servers.begin(); serverConfig != _servers.end(); ++serverConfig) {
		for (size_t i = 0; i < serverConfig->hosts.size(); ++i) {
			const std::pair<std::string, std::string>& hostPair = serverConfig->hosts[i];
			hostsToBind[hostPair.second].insert(hostPair.first);
		}
	}

	int listenSocket;

	std::map<std::string, std::set<std::string> >::iterator it = hostsToBind.begin();
	for (; it != hostsToBind.end(); ++it) {
		std::set<std::string>& hostsOfPort = it->second;
		// If there is 0.0.0.0 for this port, bind only to 0.0.0.0
		if (hostsOfPort.find(std::string("0.0.0.0")) != hostsOfPort.end()) {
			listenSocket = createListenSocket("0.0.0.0", it->first);
			_listenSockets.push_back(listenSocket);
		} else {
			// 1 or multiple (but distinct) hosts binded to the same port
			for (std::set<std::string>::iterator host = hostsOfPort.begin(); host != hostsOfPort.end(); ++host) {
				listenSocket = createListenSocket(*host, it->first);
				_listenSockets.push_back(listenSocket);
			}
		}
	}

	_maxFd = listenSocket;
}

ConnectionHandler::~ConnectionHandler() {
	for (std::list<int>::iterator it = _listenSockets.begin(); it != _listenSockets.end(); ++it) {
		close(*it);
	}

	for (std::map<int, ConnectionBuffer>::iterator it = _activeConnections.begin(); it != _activeConnections.end(); ++it) {
		close(it->first);
	}
}

Server &ConnectionHandler::findServer(Request& request) {
	bool reqHostIsIPv4 = wsutils::isIPv4(request.getHost());
	
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
        Server& currentServer = *it;

		if (reqHostIsIPv4 == true) {
			// Check hosts if it's an ip address
			for (size_t j = 0; j < currentServer.hosts.size(); ++j) {
				std::pair<std::string, std::string> &hostPair = currentServer.hosts[j];
				if (wsutils::isCorrectIP(request.getHost(), hostPair.first)
					&& request.getPort() == hostPair.second) {
					return currentServer;
				}
			}
		} else {
			// Check server names
			if (find(currentServer.server_name.begin(), currentServer.server_name.end(), request.getHost()) != currentServer.server_name.end()) {
				return currentServer;
			}
		}
        
    }

	throw Response::InvalidServerException();
}

bool ConnectionHandler::handleChunkedRequest(int connectionSocket, bool newEvent) {
	ConnectionBuffer& conn = _activeConnections[connectionSocket];
	char buffer[READ_BUFFER_SIZE];

	if (newEvent == true) {
		memset(buffer, 0, READ_BUFFER_SIZE);
		ssize_t bytesRead = recv(connectionSocket, buffer, READ_BUFFER_SIZE, 0);

		if (bytesRead < 0) {
			wsutils::warningOutput(strerror(errno));
			return false;
		} else if (bytesRead == 0) {
			// Client closed the connection
			return false;
		}

		conn.requestString.append(buffer, bytesRead);
	}

	// Receive the body until 0\r\n\r\n, unchunk it, set Content-Length, and setBody()
	size_t chunkEnd = conn.requestString.find("0\r\n\r\n");
	if (chunkEnd != std::string::npos) {
		std::string chunkedRequest = conn.requestString.substr(0, chunkEnd + 5);
		conn.requestString = conn.requestString.substr(chunkEnd + 5);

		std::string body = wsutils::unchunkRequest(chunkedRequest);

		conn.request->getHeaderMap()["Content-Length"] = wsutils::toString(body.length());
		conn.request->setBody(body);
		Server& responseServer = findServer(*(conn.request));
		conn.responseString = Response::generateResponse(*(conn.request), responseServer);
		conn.readyToResponse = true;
		delete conn.request;
		conn.request = NULL;
		conn.isChunkedRequest = false;
	}

	return true;
}

bool ConnectionHandler::receiveMsgBody(int connectionSocket, bool newEvent) {
	ConnectionBuffer& conn = _activeConnections[connectionSocket];
	size_t contentLength = wsutils::stringToNumber<size_t>(conn.request->getHeader("Content-Length"));
	
	if (newEvent == true) {
		size_t recvLength = contentLength - conn.requestString.length();
		size_t bufferSize = recvLength + 1;
		char *buffer = new char[bufferSize];

		memset(buffer, 0, bufferSize);
		ssize_t bytesRead = recv(connectionSocket, buffer, bufferSize, 0);

		conn.requestString.append(buffer, bytesRead);
		delete[] buffer;

		if (bytesRead < 0) {
			wsutils::warningOutput(strerror(errno));
			return false;
		} else if (bytesRead == 0) {
			// Client closed the connection
			FD_CLR(connectionSocket, &_readFds);
			close(connectionSocket);
			return false;
		}
	}

	// If body is all received
	if (conn.requestString.length() >= contentLength) {
		conn.request->setBody(conn.requestString.substr(0, contentLength));
		conn.requestString = conn.requestString.substr(contentLength);
		Server& responseServer = findServer(*(conn.request));
		conn.responseString = Response::generateResponse(*(conn.request), responseServer);
		conn.readyToResponse = true;
		delete conn.request;
		conn.request = NULL;
		conn.waitingForMsgBody = false;
	}

	return true;
}

/**
 * @brief handle the read event of the connection socket
 * 
 * @return true for nothing, false for closing this connection socket
 * 
*/
bool ConnectionHandler::connectionSocketRecv(int connectionSocket) {
	ConnectionBuffer& conn = _activeConnections[connectionSocket];

	if (conn.isChunkedRequest == true) {
		return handleChunkedRequest(connectionSocket, true);
	} else if (conn.waitingForMsgBody == true) {
		return receiveMsgBody(connectionSocket, true);
	}
	
	char buffer[READ_BUFFER_SIZE];
	memset(buffer, 0, READ_BUFFER_SIZE);
	ssize_t bytesRead = recv(connectionSocket, buffer, READ_BUFFER_SIZE, 0);
	
	if (bytesRead < 0) {
		wsutils::warningOutput(strerror(errno));
		return false;
	} else if (bytesRead == 0) {
		// Client closed the connection
		return false;
	} else {
		conn.requestString.append(buffer, bytesRead);
		
		size_t terminator = conn.requestString.find(HTTP_REQUEST_TERMINATOR);
		if (terminator != std::string::npos) {
			conn.request = new Request(conn.requestString.substr(0, terminator + 4));
			
			// std::cout << conn.requestString.substr(0, terminator + 4) << std::endl;

			conn.requestString = conn.requestString.substr(terminator + 4);

			std::map<std::string, std::string>& headers = conn.request->getHeaderMap();
			if (headers.find("Content-Length") != headers.end()) {
				conn.waitingForMsgBody = true;
			} else if ((headers.find("Transfer-Encoding") != headers.end()) && (headers["Transfer-Encoding"] == "chunked")) {
				conn.isChunkedRequest = true;
			} else {
				Server& responseServer = findServer(*(conn.request));
				conn.responseString = Response::generateResponse(*(conn.request), responseServer);
				conn.readyToResponse = true;
				delete conn.request;
				conn.request = NULL;
			}
		}
		
		if (conn.waitingForMsgBody == true) {
			return receiveMsgBody(connectionSocket, false);
		} else if (conn.isChunkedRequest == true) {
			return handleChunkedRequest(connectionSocket, false);
		}
	}

	return true;
}

void ConnectionHandler::initFds(void) {
	// clear all states in read and write fd sets
	FD_ZERO(&_readFds);
	FD_ZERO(&_writeFds);

	// reset _maxFd
	_maxFd = -1;
	
	// monitor all listening sockets
	// increment _maxFd if needed
	for (std::list<int>::iterator lsock = _listenSockets.begin(); lsock != _listenSockets.end(); ++lsock) {
		FD_SET(*lsock, &_readFds);
		if (*lsock > _maxFd) {
			_maxFd = *lsock;
		}
	}

	// for active connections:
	// if ready to response, add to write fd set
	// if not yet ready, add to read fd set
	// increment _maxFd if needed
	for (std::map<int, ConnectionBuffer>::iterator it = _activeConnections.begin(); it != _activeConnections.end(); ++it) {
		if (it->second.readyToResponse == true) {
			FD_SET(it->first, &_writeFds);
		} else {
			FD_SET(it->first, &_readFds);
		}

		if (it->first > _maxFd) {
			_maxFd = it->first;
		}
	}
}


/**
 * Use select to listen on multiple listen sockets and connection sockets
 * After select returns:
 * 	if the event is on a listen socket, create a new connection socket
 * 	if the event is on a connection socket
 *  - if the event is for read: read the client's data
 *  - if the event is for write: send the buffered response to client
 * 
 * When a connection socket reaches a "\r\n\r\n", buffer the Http header
 * - If have Content-Length, recv body in the next recvs
 * - If have Transfer-Encoding: chunked, recv chunked requests
 * When a connection socket recv() returns 0, means client closed the connection.
*/
void ConnectionHandler::serverListen(void) {
	// Forever listen for new connection or new data to be read
	while (true) {
		initFds();
		// std::cout << "Active Connections Left: " + wsutils::toString(_activeConnections.size()) << std::endl;
		// std::cout << "Max FD: " + wsutils::toString(_maxFd) << std::endl;

		int nready = select(_maxFd + 1, &_readFds, &_writeFds, NULL, NULL);
		if (nready == -1) {
			wsutils::errorExit(strerror(errno));
		}

		// Check for listen sockets events
		for (std::list<int>::iterator lsock = _listenSockets.begin(); lsock != _listenSockets.end(); ++lsock) {
			int listenSocket = *lsock;
			if (FD_ISSET(listenSocket, &_readFds)) {
				createNewConnection(listenSocket);
			}
		}

		// Check for connection sockets events
		std::list<int> socketsToErase;
		for (std::map<int, ConnectionBuffer>::iterator it = _activeConnections.begin(); it != _activeConnections.end(); ++it) {
			int connectionSocket = it->first;

			// write events (ready to send response back to client)
			if (FD_ISSET(connectionSocket, &_writeFds)) {
				std::string& responseString = _activeConnections[connectionSocket].responseString;
				send(connectionSocket, responseString.c_str(), responseString.length(), 0);
				_activeConnections[connectionSocket].readyToResponse = false;
				_activeConnections[connectionSocket].responseString = "";
			}
			
			// read events (ready to read data sent from client)
			else if (FD_ISSET(connectionSocket, &_readFds)) {
				try {
					if (connectionSocketRecv(connectionSocket) == false) {
						socketsToErase.push_back(connectionSocket);
					}
				} catch (Response::InvalidServerException& e) {
					socketsToErase.push_back(connectionSocket);
				}
			}
		}

		for (std::list<int>::iterator it = socketsToErase.begin(); it != socketsToErase.end(); ++it) {
			close(*it);
			_activeConnections.erase(*it);
		}
		socketsToErase.clear();
	}
}

void ConnectionHandler::createNewConnection(int listenSocket) {
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	// Create a new connection socket from the listenSocket that has connection
	int connectionSocket = accept(listenSocket, (struct sockaddr *)(&clientAddr), &clientLen);
	if (connectionSocket == -1) {
		wsutils::warningOutput(strerror(errno));
		return;
	}

	// Add the conenction socket to readFds
	FD_SET(connectionSocket, &_readFds);
	
	if (connectionSocket > _maxFd) {
		_maxFd = connectionSocket;
	}

	_activeConnections[connectionSocket] = ConnectionBuffer();

	// std::cout << "Accepted connection from " << inet_ntoa(clientAddr.sin_addr) << std::endl;
}

int ConnectionHandler::createListenSocket(std::string host, std::string port) const {
	struct addrinfo hints, *addressInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;  // Use IPv4
	hints.ai_socktype = SOCK_STREAM;  // Use TCP

	int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &addressInfo);
	if (status != 0) {
		wsutils::errorExit(gai_strerror(status));
	}

	// create the listening socket
	int listenSocket = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol);
	if (listenSocket == -1) {
		freeaddrinfo(addressInfo);
		wsutils::errorExit(strerror(errno));
	}

	// set the socket to be able to reuse immediately
	int sockopt;
	if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) == -1) {
		close(listenSocket);
		freeaddrinfo(addressInfo);
		wsutils::errorExit(strerror(errno));
	}

	// bind the socket to the host and port in the config
	if (bind(listenSocket, addressInfo->ai_addr, addressInfo->ai_addrlen) == -1) {
		close(listenSocket);
		freeaddrinfo(addressInfo);
		wsutils::errorExit(strerror(errno));
	}

	freeaddrinfo(addressInfo);

	// mark the listening socket as a passive socket (for accepting requests)
	if (listen(listenSocket, SOMAXCONN) == -1) {
		close(listenSocket);
		wsutils::errorExit(strerror(errno));
	}

	std::cerr << "Binded " << host << ':' << port << std::endl;

	return listenSocket;
}