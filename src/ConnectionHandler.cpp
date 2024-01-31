/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 18:00:03 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/31 09:37:32 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionHandler.hpp"
#include "webserv.hpp"


ConnectionBuffer::ConnectionBuffer() {
	requestString = "";
	request = NULL;
	hasUnhandledHeader = false;
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

	char commonBuffer[1024];
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
				ConnectionBuffer& conn = _activeConnections[connectionSocket];
				char *buffer;
				size_t bufferSize;
				bool bufferIsHeap;

				if (conn.hasUnhandledHeader == true) {
					size_t contentLength = wsutils::stringToNumber<size_t>(conn.request->getHeader("Content-Length"));
					size_t recvLength = contentLength - conn.requestString.length();
					buffer = new char[recvLength + 1];
					bufferSize = recvLength + 1;
					bufferIsHeap = true;
				} else {
					buffer = commonBuffer;
					bufferSize = COMMON_BUFFER_SIZE;
					bufferIsHeap = false;
				}
				
				memset(buffer, 0, bufferSize);
				ssize_t bytesRead = recv(connectionSocket, buffer, bufferSize, 0);
				
				if (bytesRead < 0) {
					wsutils::warningOutput(strerror(errno));
				} else if (bytesRead == 0) {
					// Client closed the connection
					FD_CLR(connectionSocket, &_readFds);
					close(connectionSocket);
					socketsToErase.push_back(connectionSocket);
				} else {
					conn.requestString.append(buffer, bytesRead);
					if (bufferIsHeap == true) {
						delete buffer;
					}
					
					if (conn.hasUnhandledHeader == false) {
						size_t terminator = conn.requestString.find(HTTP_REQUEST_TERMINATOR);
						if (terminator != std::string::npos) {
							conn.request = new Request(conn.requestString.substr(0, terminator + 4));
							
							std::cout << conn.requestString.substr(0, terminator + 4) << std::endl;

							conn.requestString = conn.requestString.substr(terminator + 4);
							// terminator = conn.requestString.find(HTTP_REQUEST_TERMINATOR);

							std::map<std::string, std::string>& headers = conn.request->getHeaderMap();
							if (headers.find("Content-Length") != headers.end()) {
								conn.hasUnhandledHeader = true;
							} else {
								std::string response = Response::generateResponse(*(conn.request), this->_servers);
								delete conn.request;
								conn.request = NULL;
								send(connectionSocket, response.c_str(), response.length(), 0);
							}
						}
					}
					
					if (conn.hasUnhandledHeader == true) {
						size_t contentLength = wsutils::stringToNumber<size_t>(conn.request->getHeader("Content-Length"));
						if (conn.requestString.length() >= contentLength) {
							conn.request->setBody(conn.requestString.substr(0, contentLength));
							conn.requestString = conn.requestString.substr(contentLength);
							std::string response = Response::generateResponse(*(conn.request), this->_servers);
							delete conn.request;
							conn.request = NULL;
							conn.hasUnhandledHeader = false;
							send(connectionSocket, response.c_str(), response.length(), 0);
						}
					}
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

	std::cout << "Accepted connection from " << inet_ntoa(clientAddr.sin_addr) << std::endl;
}

int ConnectionHandler::createListenSocket(std::string host, std::string port) const {
	struct addrinfo hints, *addressInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;  // Use IPv4
	hints.ai_socktype = SOCK_STREAM;  // Use TCP

	int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &addressInfo);
	if (status != 0) {
		wsutils::errorExit(strerror(errno));
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