/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 18:27:18 by zwong             #+#    #+#             */
/*   Updated: 2024/01/19 18:16:26 by zwong            ###   ########.fr       */
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
        static std::string generateResponse(Request &request, std::map<int, Server> &server);

    private:
        static std::string getContentType(const std::string& filePath);
        static bool isStaticContent(Location *location);
        static std::string handleStaticContent(const Request& request);
        static std::string readFile(const std::string& filename);
        static std::string get_resource_path(Request &request, Location &location);
        static bool isStaticContent(Location &location);
        static Location* Response::get_location(Request& request, Server& server);
        static Server &Response::find_server(Request& request, std::map<int, Server>& servers);
        static std::string Response::parse_error_pages(std::string error, std::string description, Server &server);

        static std::string handle_GET_request(Request &request, Server &server);
        static std::string handle_POST_request(Request &request, Server &server);
        static std::string handle_DELETE_request(Request &request, Server &server);
};

#endif