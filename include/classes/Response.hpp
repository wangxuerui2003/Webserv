/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 18:27:18 by zwong             #+#    #+#             */
/*   Updated: 2024/01/23 12:44:51 by zwong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <fstream>
#include <sstream>
#include "Request.hpp"
#include "Parser.hpp"

struct Location;
struct Server;

class Response {
    public:
        // although not used
        Response();
        Response(const Response& copy);
		Response& operator=(const Response& copy);
        ~Response();

        static std::string generateResponse(Request &request, std::map<int, Server> &server);

        static std::string getContentType(const std::string& filePath);
        static bool isStaticContent(Location *location);
        static Path find_default_index(Path &abs_path, Location *location);
        static std::string handleStaticContent(const Request& request, Location *location, Server &server);
        static std::string readFile(const std::string& filename, Server &server);
        static Server &find_server(Request& request, std::map<int, Server>& servers);
        static std::string parse_error_pages(std::string error, std::string description, Server &server);
        static std::string parse_custom_error_pages(std::string error, std::map<std::string, Path> &error_pages);
        
        static std::string handle_GET_request(Request &request, Location *location, Server &server);
        static std::string handle_POST_request(Request &request, Location *location, Server &server);
        static std::string handle_DELETE_request(Request &request, Location *location, Server &server);
};

#endif