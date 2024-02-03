/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 11:47:59 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/03 16:31:03 by wxuerui          ###   ########.fr       */
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

	Parser& parser = Parser::getInstance();
	try {
		if (ac == 2) {
			parser.parse(av[1]);
		} else {
			parser.parse(DEFAULT_CONFIG);
		}
	} catch (Path::InvalidPathException& e) {
		wsutils::errorExit(e.what());
	} catch (Path::InvalidOperationException& e) {
		wsutils::errorExit(e.what());
	}
	parser.print_values(parser.getServers());

	const std::vector<Server>& servers = parser.getServers();

	AConnectionHandler *connectionHandler;
	if (parser.getEventHandlerType() == SELECT) {
		connectionHandler = new Select(servers);
	} else {
		connectionHandler = new Poll(servers);
	}

	// Infinite loop handle connections
	connectionHandler->serverListen();

	// although will never execute
	delete connectionHandler;

	return 0;
}
