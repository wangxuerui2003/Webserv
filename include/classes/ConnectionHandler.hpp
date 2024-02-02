/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 17:56:29 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/02 16:40:39 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTIONHANDLER_HPP
#define CONNECTIONHANDLER_HPP

#include "Parser.hpp"
#include "Path.hpp"
#include "utils.hpp"
#include "Response.hpp"

#include <algorithm>
#include <list>
#include <set>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>


struct ConnectionBuffer {
	ConnectionBuffer();
	~ConnectionBuffer();

	std::string requestString;
	Request *request;
	bool waitingForMsgBody;
	bool isChunkedRequest;

	bool readyToResponse;
	std::string responseString;
};


class ConnectionHandler {
	public:
		ConnectionHandler(const std::vector<Server>& servers);
		~ConnectionHandler();

		int createListenSocket(std::string host, std::string port) const;
		void createNewConnection(int listenSocket);
		Server &findServer(Request& request);
		bool connectionSocketRecv(int connectionSocket);
		bool handleChunkedRequest(int connectionSocket, bool newEvent);
		bool receiveMsgBody(int connectionSocket, bool newEvent);

		void initFds(void);
		void serverListen(void);

	private:
		// connectionSocket => Read buffer (waiting for full http request end with \r\n\r\n)
		std::map<int, ConnectionBuffer> _activeConnections;
		std::list<int> _listenSockets;
		std::vector<Server> _servers;

		// // listenSocket => Server config struct
		// std::map<int, Server*> _listenServers;
		
		fd_set _readFds;
		fd_set _writeFds;
		int _maxFd;
};

#endif