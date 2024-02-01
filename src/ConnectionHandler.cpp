/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 18:00:03 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/01 10:48:25 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionHandler.hpp"
#include "webserv.hpp"


ConnectionBuffer::ConnectionBuffer() {
	requestString = "";
	request = NULL;
	waitingForMsgBody = false;
	isChunkedRequest = false;
}

ConnectionBuffer::~ConnectionBuffer() {
	delete request;
}

/**
 * Generate listening sockets for each Server object
 * And map the listening socket to it's corresponding Server object
*/
ConnectionHandler::ConnectionHandler(const std::vector<Server>& servers) {
	int listenSocket;
	for (std::vector<Server>::const_iterator serverConfig = servers.begin(); serverConfig != servers.end(); ++serverConfig) {
		for (size_t i = 0; i < serverConfig->hosts.size(); ++i) {
			listenSocket = createListenSocket(serverConfig->hosts[i].first, serverConfig->hosts[i].second);
			_servers[listenSocket] = *serverConfig;
		}
	}

	_maxFd = listenSocket;
}

ConnectionHandler::~ConnectionHandler() {
	for (std::map<int, Server>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		close(it->first);
	}

	for (std::map<int, ConnectionBuffer>::iterator it = _activeConnections.begin(); it != _activeConnections.end(); ++it) {
		close(it->first);
	}
}

std::string unchunkRequest(std::string& req) {
	std::istringstream input(req);
    std::ostringstream output;

    std::string line;
    while (!input.eof()) {
        std::getline(input, line);

        // Parse the hexadecimal chunk size
        std::istringstream sizeStream(line);
        size_t chunkSize = 0;
        sizeStream >> std::hex >> chunkSize;

        if (chunkSize == 0) {
            // End of chunks
            break;
        }

        // Read the chunk data
        char buffer[chunkSize + 1];
        input.read(buffer, chunkSize);
        buffer[chunkSize] = '\0';

        // Append the chunk data to the output
        output.write(buffer, chunkSize);

        // Read and discard the CRLF following the chunk
        input.ignore(2);
    }

    return output.str();
}

bool ConnectionHandler::handleChunkedRequest(int connectionSocket, char commonBuffer[COMMON_BUFFER_SIZE], bool newEvent) {
	ConnectionBuffer& conn = _activeConnections[connectionSocket];

	if (newEvent == true) {
		memset(commonBuffer, 0, COMMON_BUFFER_SIZE);
		ssize_t bytesRead = recv(connectionSocket, commonBuffer, COMMON_BUFFER_SIZE, 0);

		if (bytesRead < 0) {
			wsutils::warningOutput(strerror(errno));
		} else if (bytesRead == 0) {
			// Client closed the connection
			FD_CLR(connectionSocket, &_readFds);
			close(connectionSocket);
			return false;
		}

		conn.requestString.append(commonBuffer, bytesRead);
	}

	// Receive the body until 0\r\n\r\n, unchunk it, set Content-Length, and setBody()
	size_t chunkEnd = conn.requestString.find("0\r\n\r\n");
	if (chunkEnd != std::string::npos) {
		std::string chunkedRequest = conn.requestString.substr(0, chunkEnd + 5);
		conn.requestString = conn.requestString.substr(chunkEnd + 5);

		std::string body = unchunkRequest(chunkedRequest);

		conn.request->getHeaderMap()["Content-Length"] = wsutils::toString(body.length());
		conn.request->setBody(body);
		std::string response = Response::generateResponse(*(conn.request), this->_servers);
		delete conn.request;
		conn.request = NULL;
		conn.isChunkedRequest = false;
		send(connectionSocket, response.c_str(), response.length(), 0);
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
		std::string response = Response::generateResponse(*(conn.request), this->_servers);
		delete conn.request;
		conn.request = NULL;
		conn.waitingForMsgBody = false;
		send(connectionSocket, response.c_str(), response.length(), 0);
	}

	return true;
}


bool ConnectionHandler::handleConnectionSocketEvent(int connectionSocket, char commonBuffer[COMMON_BUFFER_SIZE]) {
	ConnectionBuffer& conn = _activeConnections[connectionSocket];

	if (conn.isChunkedRequest == true) {
		return handleChunkedRequest(connectionSocket, commonBuffer, true);
	} else if (conn.waitingForMsgBody == true) {
		return receiveMsgBody(connectionSocket, true);
	}
	
	memset(commonBuffer, 0, COMMON_BUFFER_SIZE);
	ssize_t bytesRead = recv(connectionSocket, commonBuffer, COMMON_BUFFER_SIZE, 0);
	
	if (bytesRead < 0) {
		wsutils::warningOutput(strerror(errno));
	} else if (bytesRead == 0) {
		// Client closed the connection
		FD_CLR(connectionSocket, &_readFds);
		close(connectionSocket);
		return false;
	} else {
		conn.requestString.append(commonBuffer, bytesRead);
		
		size_t terminator = conn.requestString.find(HTTP_REQUEST_TERMINATOR);
		if (terminator != std::string::npos) {
			conn.request = new Request(conn.requestString.substr(0, terminator + 4));
			
			std::cout << conn.requestString.substr(0, terminator + 4) << std::endl;

			conn.requestString = conn.requestString.substr(terminator + 4);

			std::map<std::string, std::string>& headers = conn.request->getHeaderMap();
			if (headers.find("Content-Length") != headers.end()) {
				conn.waitingForMsgBody = true;
			} else if ((headers.find("Transfer-Encoding") != headers.end()) && (headers["Transfer-Encoding"] == "chunked")) {
				conn.isChunkedRequest = true;
			} else {
				std::string response = Response::generateResponse(*(conn.request), this->_servers);
				delete conn.request;
				conn.request = NULL;
				send(connectionSocket, response.c_str(), response.length(), 0);
			}
		}
		
		if (conn.waitingForMsgBody == true) {
			return receiveMsgBody(connectionSocket, false);
		} else if (conn.isChunkedRequest == true) {
			return handleChunkedRequest(connectionSocket, commonBuffer, false);
		}
	}

	return true;
}


/**
 * Use select to listen on multiple listen sockets and connection sockets
 * After select returns:
 * 	if the event is on a listen socket, create a new connection socket
 * 	if the event is on a connection socket, read the client's data
 * 
 * When a connection socket reaches a "\r\n\r\n", handle the HTTP request.
 * When a connection socket recv() returns 0, means client closed the connection.
*/
void ConnectionHandler::serverListen(void) {
	fd_set tempFds;

	// Let the kernel to monitor all listening sockets events
	for (std::map<int, Server>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		int listenSocket = it->first;
		FD_SET(listenSocket, &_readFds);
	}

	char commonBuffer[COMMON_BUFFER_SIZE];
	// Forever listen for new connection or new data to be read
	while (true) {
		tempFds = _readFds;

		// Monitor read event only
		int nready = select(_maxFd + 1, &tempFds, NULL, NULL, NULL);
		if (nready == -1) {
			wsutils::errorExit(strerror(errno));
		}

		// Check for listen sockets events
		for (std::map<int, Server>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
			int listenSocket = it->first;
			if (FD_ISSET(listenSocket, &tempFds)) {
				createNewConnection(listenSocket);
			}
		}

		// Check for connection sockets events
		std::vector<int> socketsToErase;
		for (std::map<int, ConnectionBuffer>::iterator it = _activeConnections.begin(); it != _activeConnections.end(); ++it) {
			int connectionSocket = it->first;
			if (FD_ISSET(connectionSocket, &tempFds)) {
				if (handleConnectionSocketEvent(connectionSocket, commonBuffer) == false) {
					// connection has been closed
					socketsToErase.push_back(connectionSocket);
				}
			}
		}

		for (std::vector<int>::iterator it = socketsToErase.begin(); it != socketsToErase.end(); ++it) {
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

	return listenSocket;
}