/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 17:48:35 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/13 18:19:38 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

// Initialize the instance pointer
Parser *Parser::_instance = NULL;

Parser::Parser() {
}

Parser *Parser::getInstance() {
	if (_instance == NULL) {
		_instance = new Parser();
	}

	return _instance;
}

const std::vector<Server>& Parser::getServers(void) const {
	return _servers;
}

void Parser::parse(std::string configFilePath) {
	Path configFile(configFilePath, REG_FILE);
	// TODO: Parse the config file into the Server objects

	/***********************************/
	/************DUMMY CONF*************/
	/***********************************/

	Server dummy;
	dummy.root = Path("./www", DIRECTORY);
	dummy.host = "127.0.0.1";
	dummy.port = 8080;
	dummy.index.push_back("index.html");
	dummy.error_pages[404] = Path("./www/404.html", REG_FILE);

	Location location;
	location.url = "/";
	location.root = dummy.root;
	location.index = dummy.index;

	dummy.locations.push_back(location);

	_servers.push_back(dummy);
}

Server::Server() {}

Location::Location() {}