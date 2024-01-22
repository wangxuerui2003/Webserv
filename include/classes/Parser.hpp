/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 17:55:47 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/15 17:29:31 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "Path.hpp"

struct Location {
	Location();

	std::vector<std::string> path;
	std::vector<std::string> root;
	// std::string url;
	// Path root;
	// std::vector<std::string> allowedHttpMethods;
	// bool directory_listing;
	// std::vector<std::string> index;
};

struct Server {
	Server();

	std::vector<std::string> listen;
	std::vector<std::string> root;
	std::vector<std::string> index;
	std::vector<std::string> server_name;
	std::vector<std::string> error_page;
	// std::string port;
	// std::string host;
	// std::string server_name;
	// Path root;
	// std::vector<std::string> index;
	// std::map<int, Path> error_pages;
	// size_t max_client_body_size;

	std::vector<Location> locations;
};

// Singleton Parser class
class Parser {
	public:
		static Parser *getInstance(void);

		const std::vector<Server>& getServers(void) const;
	
		std::vector<std::string> getKeywordValues(std::string keyword, std::vector<std::string> serverLines);

		void parse(std::string configFilePath);

	private:
		Parser();

		// Disable copying
		Parser(const Parser& copy);
		Parser& operator=(const Parser& copy);

		static Parser *_instance;  // the only instance in the global scope

		std::vector<Server> _servers;
};



#endif