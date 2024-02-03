/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Select.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 11:43:50 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/03 11:52:58 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Select.hpp"

Select::Select(const std::vector<Server>& servers) : AConnectionHandler(servers) {

}

Select::~Select() {
	
}


void Select::initFds(void) {
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
void Select::serverListen(void) {
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
				int bytesSent = send(connectionSocket, responseString.c_str(), responseString.length(), 0);
				if (bytesSent < 0) {
					socketsToErase.push_back(connectionSocket);
				} else if (bytesSent < static_cast<int>(responseString.length())) {
					responseString = responseString.substr(bytesSent - 1);
				} else {
					_activeConnections[connectionSocket].responseString = "";
					_activeConnections[connectionSocket].readyToResponse = false;
				}
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