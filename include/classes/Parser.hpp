/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 17:55:47 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/30 17:04:30 by wxuerui          ###   ########.fr       */
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

	Path uri;
	Path root;
	std::vector<std::string> index;
	bool autoindex;
	size_t max_client_body_size;
	Path upload_store;
	bool accept_upload;
	std::vector<std::string> allowedHttpMethods;
};

struct Server {
	Server();

	std::vector<std::pair<std::string, std::string> > hosts;
	std::vector<std::string> server_name;
	Path root;
	std::vector<std::string> index;
	std::map<std::string, Path> error_pages;
	std::vector<std::string> cgi_extensions;

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