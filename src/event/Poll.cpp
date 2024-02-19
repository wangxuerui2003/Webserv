/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poll.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 14:25:42 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/19 13:21:35 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Poll.hpp"

Poll::Poll(const std::vector<Server>& servers) : AConnectionHandler(servers) {

}

Poll::~Poll() {
	
}

void	Poll::initFds(void) {
	// Resize to just enough size for all listening and connection sockets
	_monitorFds.resize(_listenSockets.size() + _activeConnections.size());
	int currPollFd = 0;

	// monitor all listening sockets on read events only
	// increment _maxFd if needed
	for (std::list<int>::iterator lsock = _listenSockets.begin(); lsock != _listenSockets.end(); ++lsock) {
		_monitorFds[currPollFd].fd = *lsock;
		_monitorFds[currPollFd].events = POLLIN;
		currPollFd++;
	}

	// for active connections:
	// if ready to response, monitor write events
	// if not yet ready, monitor read events
	// increment _maxFd if needed
	for (std::map<int, ConnectionBuffer>::iterator it = _activeConnections.begin(); it != _activeConnections.end(); ++it) {
		_monitorFds[currPollFd].fd = it->first;
		if (it->second.readyToResponse == true) {
			_monitorFds[currPollFd].events = POLLOUT;
		} else {
			_monitorFds[currPollFd].events = POLLIN;
		}

		_monitorFds[currPollFd].events |= POLLERR;
		_monitorFds[currPollFd].events |= POLLHUP;

		currPollFd++;
	}
}


/**
 * Use poll to listen on multiple listen sockets and connection sockets
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
void	Poll::serverListen(void) {
	// Forever listen for new connection or new data to be read
	while (true) {
		initFds();
		// std::cout << "Active Connections Left: " + wsutils::toString(_activeConnections.size()) << std::endl;

		int nready = poll(_monitorFds.data(), _monitorFds.size(), -1);  // -1 for block forever if no event
		if (nready == -1) {
			wsutils::errorExit(strerror(errno));
		}

		std::list<int> socketsToErase;
		std::vector<struct pollfd>::iterator it;
		for (it = _monitorFds.begin(); it != _monitorFds.end() && nready > 0; ++it) {
			// current fd has event returned
			if (it->revents != 0) {
				--nready;
				int eventSock = it->fd;

				// for listen sockets, only have read events, no need to check revents
				if (std::find(_listenSockets.begin(), _listenSockets.end(), eventSock) != _listenSockets.end()) {
					createNewConnection(eventSock);
				}
				
				// connection socket event
				else {
					short revent = it->revents;

					// error events
					if ((revent & POLLERR) || (revent & POLLHUP)) {
						socketsToErase.push_back(eventSock);
					}

					// write events (ready to send response back to client)
					else if (revent & POLLOUT) {
						std::string& responseString = _activeConnections[eventSock].responseString;
						// std::cout << responseString << std::endl;
						int bytesSent = send(eventSock, responseString.c_str(), responseString.length(), 0);
						if (bytesSent < 0) {
							socketsToErase.push_back(eventSock);
						} else if (bytesSent < static_cast<int>(responseString.length())) {
							responseString = responseString.substr(bytesSent - 1);
						} else {
							_activeConnections[eventSock].responseString = "";
							_activeConnections[eventSock].readyToResponse = false;
						}
					}
					
					// read events (ready to read data sent from client)
					else if (revent & POLLIN) {
						try {
							if (connectionSocketRecv(eventSock) == false) {
								socketsToErase.push_back(eventSock);
							}
						} catch (Response::InvalidServerException& e) {
							socketsToErase.push_back(eventSock);
						}
					}

				}
			}
		}

		for (std::list<int>::iterator it = socketsToErase.begin(); it != socketsToErase.end(); ++it) {
			close(*it);
			_activeConnections.erase(*it);
		}
	}
}