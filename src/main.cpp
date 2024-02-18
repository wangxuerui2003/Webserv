/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:47:59 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/18 15:08:58 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Select.hpp"
#include "Poll.hpp"
#include "Parser.hpp"

int main(int ac, char **av) {
	if (ac > 2) {
		wsutils::errorExit(USAGE);
	}

	std::string configPath;
	if (ac == 2) {
		configPath = av[1];
	} else {
		configPath = DEFAULT_CONFIG;
	}

	Config config;
	try {
		Parser::parse(configPath, config);
	} catch (Path::InvalidPathException& e) {
		wsutils::errorExit(e.what());
	} catch (Path::InvalidOperationException& e) {
		wsutils::errorExit(e.what());
	}

	if (config.servers.empty()) {
		wsutils::errorExit("No servers in config file");
	}
	
	Parser::print_values(config.servers);

	AConnectionHandler *connectionHandler;
	if (config.eventHandlerType == SELECT) {
		connectionHandler = new Select(config.servers);
	} else {
		connectionHandler = new Poll(config.servers);
	}

	srand(std::time(NULL));  // initialize the random seed for generating session ids

	// Infinite loop handle connections
	connectionHandler->serverListen();

	// although will never execute
	delete connectionHandler;

	return 0;
}
