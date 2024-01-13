/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:47:59 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/13 18:22:38 by wxuerui          ###   ########.fr       */
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
	handler.listen();

	return 0;
}