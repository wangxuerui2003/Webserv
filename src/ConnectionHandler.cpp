/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 18:00:03 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/13 18:45:03 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionHandler.hpp"

ConnectionHandler::ConnectionHandler(const std::vector<Server>& servers) {
	// TODO: Generate listening sockets for each Server object
	// TODO: And map the listening socket to it's corresponding Server object

	int listenSocket;
	for (std::vector<Server>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
		listenSocket = createListenSocket(*it);
		_servers[listenSocket] = *it;
	}
}

ConnectionHandler::~ConnectionHandler() {
	
}

void ConnectionHandler::listen(void) {
	// TODO: use select/poll/epoll to listen on multiple listen sockets and connection sockets

	// TODO: if the event is on a listen socket, create a new connection socket
	// TODO: if the event is on a connection socket, handle the HTTP request
}

int ConnectionHandler::createListenSocket(const Server& config) const {

	// TODO: create the listen socket

	// TODO: set the socket to be able to reuse immediately (setsockopt)

	// TODO: bind the socket to the host and port in the config

	(void)config;
	return 0;
}
