/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AConnectionHandler.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 17:56:29 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/03 11:56:27 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACONNECTIONHANDLER_HPP
#define ACONNECTIONHANDLER_HPP

#include "webserv.hpp"
#include "Response.hpp"


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


class AConnectionHandler {
	public:
		AConnectionHandler(const std::vector<Server>& servers);
		virtual ~AConnectionHandler();

		int createListenSocket(std::string host, std::string port) const;
		void createNewConnection(int listenSocket);
		Server &findServer(Request& request);
		bool connectionSocketRecv(int connectionSocket);
		bool handleChunkedRequest(int connectionSocket, bool newEvent);
		bool receiveMsgBody(int connectionSocket, bool newEvent);

		virtual void initFds(void) = 0;
		virtual void serverListen(void) = 0;

	protected:
		// connectionSocket => Read buffer (waiting for full http request end with \r\n\r\n)
		std::map<int, ConnectionBuffer> _activeConnections;

		std::list<int> _listenSockets;
		std::vector<Server> _servers;
};

#endif