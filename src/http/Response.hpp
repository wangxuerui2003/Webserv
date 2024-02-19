/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 18:27:18 by zwong             #+#    #+#             */
/*   Updated: 2024/02/19 09:34:29 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "webserv.hpp"
#include "Request.hpp"
#include "Path.hpp"
#include "Config.hpp"
#include "CgiHandler.hpp"

class Response {
    public:
        static std::string generateResponse(Request &request, Server &server);

        static std::string getContentType(const std::string& filePath);
        static bool isStaticContent(const Path& uri, Server& server);
        static Path find_default_index(Path &abs_path, Location *location);
        static std::string handleStaticContent(Request& request, Path &absPath, Location *location, Server &server);
        static std::string readFile(Path &absPath, Server &server);
        static Server &findServer(Request &request, std::map<int, Server*> &servers);
        static std::string parse_error_pages(std::string error, std::string description, Server &server);
        static std::string parse_custom_error_pages(std::string error, std::map<std::string, Path> &error_pages);
        static std::string custom_error_page(std::string error, std::string description);
        
        static std::string handle_GET_request(Request &request, Location *location, Server &server);
        static std::string handle_POST_request(Request &request, Location *location, Server &server);
        static std::string handle_DELETE_request(Path &abspath, Server &server);

        static Location *getBestFitLocation(std::vector<Location>& locations, Path& requestUri);
        static std::string generateDirectoryListing(const Path& dirPath, const Path& uri);

        class InvalidServerException : public std::exception {
			public:
				const char *what() const throw();
		};
};

#endif