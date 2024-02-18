/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 17:55:47 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/18 14:54:50 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include "webserv.hpp"
#include "Config.hpp"

class Parser {
	public:
		static void parse(std::string configFilePath, Config& config);

		static void print_server(const Server &server);
		static void print_location(const Location &location);
		static void print_values(std::vector<Server>& _servers);
	private:
		// helper functions for parse()
		static void parseEventContext(Config& config, std::vector<std::string>& configLines, size_t& currentLineIndex);
		static void parseServerContext(Config& config, std::vector<std::string>& configLines, size_t& currentLineIndex);
		static Location parseLocationContext(std::vector<std::string>& configLines, std::vector<std::string>& locationLines);
		static std::vector<std::string> getKeywordValues(std::string keyword, std::vector<std::string>& serverLines);
		static void getLineValues(std::vector<std::string>& values, std::string& keyword, std::string& line);
};



#endif