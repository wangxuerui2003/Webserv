/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 17:55:47 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/17 15:49:48 by zwong            ###   ########.fr       */
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
	std::string root;
	std::vector<std::string> index;
	std::string cgi_pass;
	// Path root;
	// std::vector<std::string> allowedHttpMethods;
	// bool directory_listing;
};

struct Server {
	Server();

	std::string port;
	std::string host;
	std::string server_name;
	Path root;
	std::vector<std::string> index;
	std::map<int, Path> error_pages;
	size_t max_client_body_size;

	std::vector<Location> locations;
};

// Singleton Parser class
class Parser {
	public:
		static Parser *getInstance(void);

		const std::vector<Server>& getServers(void) const;

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