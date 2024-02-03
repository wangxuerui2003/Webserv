/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poll.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 14:25:42 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/03 15:26:23 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Poll.hpp"

Poll::Poll(const std::vector<Server>& servers) : AConnectionHandler(servers) {

}

Poll::~Poll() {
	
}

void	Poll::initFds(void) {

	// Reset pollfds to -1 (kernel will ignore -1 fds)
	for (int i = 0; i < MAX_POLL_FDS; ++i) {
		_monitorFds[i].fd = -1;
	}

	// reset _maxFd
	_maxFd = 0;

	int currPollFd = 0;

	// monitor all listening sockets on read events only
	// increment _maxFd if needed
	for (std::list<int>::iterator lsock = _listenSockets.begin(); lsock != _listenSockets.end(); ++lsock) {
		if (currPollFd >= MAX_POLL_FDS - 1) {
			break;
		}

		_monitorFds[currPollFd].fd = *lsock;
		_monitorFds[currPollFd].events = POLLIN;
		currPollFd++;
	}

	// for active connections:
	// if ready to response, monitor write events
	// if not yet ready, monitor read events
	// increment _maxFd if needed
	for (std::map<int, ConnectionBuffer>::iterator it = _activeConnections.begin(); it != _activeConnections.end(); ++it) {
		if (currPollFd >= MAX_POLL_FDS - 1) {
			break;
		}

		_monitorFds[currPollFd].fd = it->first;
		if (it->second.readyToResponse == true) {
			_monitorFds[currPollFd].events = POLLOUT;
		} else {
			_monitorFds[currPollFd].events = POLLIN;
		}

		currPollFd++;
	}

	_maxFd = currPollFd;
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
		// std::cout << "Max FD: " + wsutils::toString(_maxFd) << std::endl;

		int nready = poll(_monitorFds, _maxFd + 1, -1);  // -1 for block forever if no event
		if (nready == -1) {
			wsutils::errorExit(strerror(errno));
		}

		std::list<int> socketsToErase;
		for (int i = 0; i < _maxFd + 1 && nready > 0; ++i) {
			// current fd has event returned
			if (_monitorFds[i].revents != 0) {
				--nready;
				int fd = _monitorFds[i].fd;

				// for listen sockets, only have read events, no need to check revents
				if (std::find(_listenSockets.begin(), _listenSockets.end(), fd) != _listenSockets.end()) {
					createNewConnection(fd);
				}
				
				// connection socket event
				else {
					short revent = _monitorFds[i].revents;

					// write events (ready to send response back to client)
					if (revent & POLLOUT) {
						std::string& responseString = _activeConnections[fd].responseString;
						int bytesSent = send(fd, responseString.c_str(), responseString.length(), 0);
						if (bytesSent < 0) {
							socketsToErase.push_back(fd);
						} else if (bytesSent < static_cast<int>(responseString.length())) {
							responseString = responseString.substr(bytesSent - 1);
						} else {
							_activeConnections[fd].responseString = "";
							_activeConnections[fd].readyToResponse = false;
						}
					}
					
					// read events (ready to read data sent from client)
					else if (revent & POLLIN) {
						try {
							if (connectionSocketRecv(fd) == false) {
								socketsToErase.push_back(fd);
							}
						} catch (Response::InvalidServerException& e) {
							socketsToErase.push_back(fd);
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