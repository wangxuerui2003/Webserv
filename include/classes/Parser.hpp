/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 17:55:47 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/03 11:58:22 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <webserv.hpp>

struct Location {
	Location();

	Path uri;
	Path root;
	bool isCustomRoot;
	std::vector<std::string> index;
	bool autoindex;
	size_t max_client_body_size;
	Path upload_store;
	bool accept_upload;
	std::vector<std::string> allowedHttpMethods;

	bool isHttpRedirection;
	std::string redirectionStatusCode;
	std::string redirectURL;
};

struct Server {
	Server();

	std::vector<std::pair<std::string, std::string> > hosts;
	std::vector<std::string> server_name;
	Path root;
	std::vector<std::string> index;
	std::map<std::string, Path> error_pages;
	std::map<std::string, std::string> cgiHandlers;

	std::vector<Location> locations;
};

// Singleton Parser class
class Parser {
	public:
		static Parser& getInstance(void);

		const std::vector<Server>& getServers(void) const;
	
		std::vector<std::string> getKeywordValues(std::string keyword, std::vector<std::string> serverLines);

		void parse(std::string configFilePath);

		void print_server(const Server &server);
		void print_location(const Location &location);
		void print_values(std::vector<Server> _servers);
	private:
		Parser();

		// Disable copying
		Parser(const Parser& copy);
		Parser& operator=(const Parser& copy);

		std::vector<Server> _servers;
};



#endif