/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 17:55:47 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/13 18:18:12 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Path.hpp"

struct Location {
	Location();
	
	std::string url;
	Path root;
	// std::vector<std::string> allowedHttpMethods;
	// bool directory_listing;
	std::vector<std::string> index;
};

struct Server {
	Server();

	short port;
	std::string host;
	std::string server_name;
	Path root;
	std::vector<std::string> index;
	std::map<int, Path> error_pages;
	// size_t max_client_body_size;

	std::vector<Location> locations;
};

// Singleton Parser class
class Parser {
	public:
		static Parser *getInstance(void);

		const std::vector<Server>& getServers(void) const;

		void parse(std::string configFilePath);

	private:
		Parser(void);

		// Disable copying
		Parser(const Parser& copy);
		Parser& operator=(const Parser& copy);

		static Parser *_instance;  // the only instance in the global scope

		std::vector<Server> _servers;
};



#endif