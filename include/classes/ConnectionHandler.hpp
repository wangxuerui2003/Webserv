/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 17:56:29 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/23 20:45:16 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTIONHANDLER_HPP
#define CONNECTIONHANDLER_HPP

#include "Parser.hpp"
#include "Path.hpp"
#include "utils.hpp"
#include "Response.hpp"

#include <algorithm>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

class ConnectionHandler {
	public:
		ConnectionHandler(const std::vector<Server>& servers);
		~ConnectionHandler();

		int createListenSocket(std::string host, std::string port) const;
		void createNewConnection(int listenSocket);

		void serverListen(void);

	private:
		// connectionSocket => Read buffer (waiting for full http request end with \r\n\r\n)
		std::map<int, std::string> _activeConnections;

		// listenSocket => Server config struct
		std::map<int, Server> _servers;
		
		fd_set _readFds;
		int _maxFd;


};

#endif