/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 18:27:18 by zwong             #+#    #+#             */
/*   Updated: 2024/02/01 18:16:32 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <fstream>
#include <sstream>
#include "Request.hpp"
#include "Parser.hpp"
#include "CgiHandler.hpp"

struct Location;
struct Server;

class Response {
    public:
        // although not used
        Response();
        Response(const Response& copy);
		Response& operator=(const Response& copy);
        ~Response();

        static std::string generateResponse(Request &request, Server &server);

        static std::string getContentType(const std::string& filePath);
        static bool isStaticContent(Path& uri, Server& server);
        static Path find_default_index(Path &abs_path, Location *location);
        static std::string handleStaticContent(Request& request, Path &absPath, Location *location, Server &server);
        static std::string readFile(Path &absPath, Server &server);
        static Server &findServer(Request &request, std::map<int, Server*> &servers);
        static std::string parse_error_pages(std::string error, std::string description, Server &server);
        static std::string parse_custom_error_pages(std::string error, std::map<std::string, Path> &error_pages);
        
        static std::string handle_GET_request(Request &request, Location *location, Server &server);
        static std::string handle_POST_request(Request &request, Location *location, Server &server);
        static std::string handle_DELETE_request(Path &abspath, Location *location, Server &server);

        static std::string deleteResource(Path &absPath, Server &server);

        class InvalidServerException : public std::exception {
			public:
				const char *what() const throw();
		};
};

#endif