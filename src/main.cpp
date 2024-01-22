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

// int main(int ac, char **av) {
// 	if (ac > 2) {
// 		wsutils::errorExit(USAGE);
// 	}

// 	Parser *parser = Parser::getInstance();
// 	try {
// 		if (ac == 2) {
// 			parser->parse(av[1]);
// 		} else {
// 			parser->parse(DEFAULT_CONFIG);
// 		}
// 	} catch (Path::InvalidPathException& e) {
// 		wsutils::errorExit("Invalid config file path");
// 	}

// 	const std::vector<Server>& servers = parser->getServers();

// 	// Spawn the servers from the configurations
// 	ConnectionHandler handler(servers);

// 	delete parser;

// 	// Infinite loop handle connections
// 	handler.listen();

// 	return 0;
// }

int main(void)
{
	Parser *parser = Parser::getInstance();
	parser->parse("./conf/example.conf");

	std::vector<Server> test = parser->getServers();
	for (std::vector<Server>::iterator it = test.begin(); it != test.end(); ++it) {
		int i = 0;
		i++;
		std::cout << "Server " << i << " - ";
		std::cout << "| listen: ";
		for (std::vector<std::string>::iterator it2 = it->listen.begin(); it2 != it->listen.end(); ++it2)
			std::cout << *it2 << " ";
		std::cout << "| root: ";
		for (std::vector<std::string>::iterator it2 = it->root.begin(); it2 != it->root.end(); ++it2)
			std::cout << *it2 << " ";
		std::cout << "| index: ";
		for (std::vector<std::string>::iterator it2 = it->index.begin(); it2 != it->index.end(); ++it2)
			std::cout << *it2 << " ";
		std::cout << "| server_name: ";
		for (std::vector<std::string>::iterator it2 = it->server_name.begin(); it2 != it->server_name.end(); ++it2)
			std::cout << *it2 << " ";
		std::cout << "| error_page: ";
		for (std::vector<std::string>::iterator it2 = it->error_page.begin(); it2 != it->error_page.end(); ++it2)
			std::cout << *it2 << " ";
		std::cout << "| locations[0].path: ";
		for (std::vector<std::string>::iterator it2 = it->locations[0].path.begin(); it2 != it->locations[0].path.end(); ++it2)
			std::cout << *it2 << " ";
		std::cout << "| locations[0].root: ";
		for (std::vector<std::string>::iterator it2 = it->locations[0].root.begin(); it2 != it->locations[0].root.end(); ++it2)
			std::cout << *it2 << " ";
		std::cout << "| locations[1].path: ";
		for (std::vector<std::string>::iterator it2 = it->locations[1].path.begin(); it2 != it->locations[1].path.end(); ++it2)
			std::cout << *it2 << " ";
		std::cout << "| locations[1].root: ";
		for (std::vector<std::string>::iterator it2 = it->locations[1].root.begin(); it2 != it->locations[1].root.end(); ++it2)
			std::cout << *it2 << " ";
		std::cout << std::endl;
    }
}