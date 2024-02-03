/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 17:55:47 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/03 13:37:25 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include "webserv.hpp"
#include "Config.hpp"

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