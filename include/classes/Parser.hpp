/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 17:55:47 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/23 12:02:28 by zwong            ###   ########.fr       */
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
#include <utility>

#include "Path.hpp"

struct Location {
	Location();

	// std::vector<std::string> path;
	// std::vector<std::string> root;
	Path uri;
	Path root;
	Path cgi_pass;
	std::vector<std::string> index;
	bool autoindex;
	size_t max_client_body_size;
	Path upload_store;
	bool accept_upload;
	std::vector<std::string> allowedHttpMethods;
};

struct Server {
	Server();

	// std::string port;
	// std::string host;
	std::vector<std::pair<std::string, std::string> > hosts;
	std::vector<std::string> server_name;
	Path root;
	std::vector<std::string> index;
	std::map<std::string, Path> error_pages;

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