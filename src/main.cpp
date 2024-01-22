/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:47:59 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/15 17:30:07 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main(int ac, char **av) {
	if (ac > 2) {
		wsutils::errorExit(USAGE);
	}

	Parser *parser = Parser::getInstance();
	try {
		if (ac == 2) {
			parser->parse(av[1]);
		} else {
			parser->parse(DEFAULT_CONFIG);
		}
	} catch (Path::InvalidPathException& e) {
		wsutils::errorExit("Invalid config file path");
	}

	const std::vector<Server>& servers = parser->getServers();

	// Spawn the servers from the configurations
	ConnectionHandler handler(servers);

	delete parser;

	// Infinite loop handle connections
	handler.serverListen();

	return 0;
}
