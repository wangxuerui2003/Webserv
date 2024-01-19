/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 18:30:17 by zwong             #+#    #+#             */
/*   Updated: 2024/01/19 18:19:01 by zwong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

std::string Response::getContentType(const std::string& filePath) {
    // Map file extensions to MIME types
    std::map<std::string, std::string> mimeTypes;
    mimeTypes["html"] = "text/html";
    mimeTypes["css"] = "text/css";
    mimeTypes["js"] = "application/javascript";
    mimeTypes["ico"] = "image/x-icon";
    mimeTypes["png"] = "image/png";
    mimeTypes["gif"] = "image/gif";
    // Add more mappings as needed

    // Get the file extension
    size_t dotPos = filePath.find_last_of(".");
    if (dotPos != std::string::npos) {
        std::string extension = filePath.substr(dotPos + 1);

        // Look up the MIME type based on the file extension
        std::map<std::string, std::string>::iterator it = mimeTypes.find(extension);
        if (it != mimeTypes.end()) {
            return it->second;
        }
    }

    // Default to plain text if the MIME type is not found
    return "text/plain";
}

std::string Response::readFile(const std::string& filename) {
    std::string response;
    std::ifstream file(filename);
    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        response += "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: " + getContentType(filename) + "\r\n";
        response += ("Content-Length: " + std::to_string(buffer.str().length()) + "\r\n");
        response += "\r\n";
        response += buffer.str();
        // std::cout << "FINAL STR: " << response << std::endl;
        return (response);
    }
    return "HTTP/1.1 404 Not Found\r\n\r\nFile not found.";
}

// IF static content, then just return string using readFile
// TODO: Use PATH object to handle static content
std::string Response::handleStaticContent(const Request& request) {
    // Handle static content based on the requested path.
    std::cout << "Handling static content..." << std::endl;

    std::string path = "www/" + request.getPath().substr(1);  // Remove the leading '/' from the path.
    std::cout << "FILE PATH: " << path << std::endl;
    // You may implement more sophisticated logic here, such as serving files based on the path.
    return (readFile(path));
    // std::string resource_path = get_resource_path(request, *location);
    // parse_resource()
    // if (this->data = "")
    // return (error);
}

// Take params of Request and std::vector<Server> servers
// Then find the server that corresponds to this request
std::string Response::generateResponse(Request &request, std::map<int, Server> &servers) {

    std::string method = request.getMethod();
    std::string path = request.getPath();
    std::string version = request.getVersion();
    
    Server server = find_server(request, servers);

    if (method != "GET" && method != "POST" && method != "DELETE")
        return (parse_error_pages("501", "Method not implemented", server));

    // Processing response    
	Location * location = get_location(request, server);
    
    if (location == NULL)
        return (parse_error_pages("403", "Forbidden", server));
    
    // if ((*location).get_return() != "") 
			// this->handle_return((*location).get_return());

    // Check if the resource is a static file
    if (isStaticContent(location)) {
        if (request.getMethod() == "GET")
            handleStaticContent(request);
        else
            return (parse_error_pages("405", "Method not allowed", server));
    }
    else {
        // Proceed with handling dynamic content with CGI
        if (request.getMethod() == "GET")
            handle_GET_request(request, server);
        else if (request.getMethod() == "POST")
            return (handle_POST_request(request, server));
        else if (request.getMethod() == "DELETE")
            handle_DELETE_request(request, server);
    }
}

// std::string	Response::get_resource_path(Request &request, Location &location) {
// 	std::string encodedString = request.getPath(); // find out route in Request header aka path
//     std::string decodedString = urlDecode(encodedString);
// 	std::string resource_path = ".";

// 	if (location.root != "")
// 		resource_path += location.root;
// 	resource_path += decodedString;
// 	return (resource_path);
// }

bool Response::isStaticContent(Location &location) {
	return (location.cgi_pass == "");
}

// EXAMPLE OF CGI_PASS
// location /dynamic {
//     # Specify the CGI endpoint for handling dynamic content
//     cgi_pass http://localhost:8080/cgi-bin;
// }
// TODO: Get location from server
std::string Response::handle_POST_request(Request &request, Server &server) {
    Location *location = get_location(request, server);

    if (location->cgi_pass == "")
        return (parse_error_pages("405", "Method not allowed ", server));
    else if (request.getHeader("Content-Length") != "" && server.max_client_body_size != NULL && // CHECK: max_client_body_size when not defined?
    std::stoull(request.getHeader("Content-Length")) > server.max_client_body_size)
        return (parse_error_pages("413", "Payload Too Large", server));
    else {
        // CgiHandler cgi;
        // cgi.handle_cgi(request, *this, server, *location);
    }
}

// std::string	Response::parse_resource_path(Request & request, Location & location) {
// 	std::string encodedString = request.get_route();
//     std::string decodedString = urlDecode(encodedString);
// 	std::string resource_path = ".";

// 	if (location.get_root() != "")
// 		resource_path += location.get_root();
// 	resource_path += decodedString;
// 	return resource_path;
// }

Location *Response::get_location(Request& request, Server& server) {
    const std::string& req_path = request.getPath();

    for (size_t index = 0; index < server.locations.size(); ++index) {
        const Location& location = server.locations[index];
        const std::string& location_root = location.root;

        if (req_path.find(location_root) == 1) {
            // Found a matching location
            return const_cast<Location*>(&location);
        }
    }

    return (NULL);  // No matching location found
}

Server &Response::find_server(Request& request, std::map<int, Server>& servers) {
    std::map<int, Server>::iterator it = servers.find(std::stoi(request.getPort())); // confirm what map<int> represents, should be port number

    if (it != servers.end()) {
        // Matching port found
        const Server& currentServer = it->second;

        if (request.getHost() == currentServer.server_name) { // e.g. example.com == example.com?
            // Matching server found
            return const_cast<Server&>(currentServer);
        }
    }

    // If no exact match is found, return the server with the same index as the first server checked
    return (servers.begin()->second);
}

// TODO: Get custom error page from server
std::string Response::parse_error_pages(std::string error, std::string description, Server &server) {
	// std::string temp_message_body = this->parse_custom_error_pages(error, server.error_pages);
    std::string temp_msg_body;
    std::string data;
    
    if (temp_msg_body == "")
        temp_msg_body.append("<html><head><title>").append(error).append(" ").append(description).append("</title></head><body><center><h1>")\
        .append(error).append(" ").append(description).append("</h1></center></body></html>");
	
    data = "HTTP/1.1 " + error + " " + description + "\r\n";
    
    data.append("Content-Type: text/html\r\n").append("Content-Length: ").append(std::to_string(temp_msg_body.length())).append("\r\n").append("\r\n");
    data.append(temp_msg_body);
    data.append("\r\n");
}