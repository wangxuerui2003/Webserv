/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 17:56:29 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/03 11:06:23 by wxuerui          ###   ########.fr       */
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

		int selectInitFds(fd_set *readFds, fd_set *writeFds);
		void serverListen(void);

	private:
		// connectionSocket => Read buffer (waiting for full http request end with \r\n\r\n)
		std::map<int, ConnectionBuffer> _activeConnections;
		std::list<int> _listenSockets;
		std::vector<Server> _servers;
};

#endif