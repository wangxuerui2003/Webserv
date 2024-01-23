/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 18:30:17 by zwong             #+#    #+#             */
/*   Updated: 2024/01/23 12:46:26 by zwong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

std::string Response::parse_custom_error_pages(std::string error, std::map<std::string, Path> &error_pages) {
	std::map<std::string, Path>::iterator head = error_pages.begin();
	std::map<std::string, Path>::iterator end = error_pages.end();
	std::string buffer;
	std::string temp_msg_body;

	for (; head != end; head++) {
		if (head->first == error) {
			std::string path = "." + head->second.getPath();
			std::fstream fs(path, std::fstream::in);
			if (fs.is_open()) {
				while(std::getline(fs, buffer, '\n'))
					temp_msg_body += buffer;
				fs.close();
			}
		}
	}
	return temp_msg_body;
}

std::string Response::parse_error_pages(std::string error, std::string description, Server &server) {
	std::string temp_msg_body = parse_custom_error_pages(error, server.error_pages);
    std::string data;
    
    if (temp_msg_body == "")
        temp_msg_body.append("<html><head><title>").append(error).append(" ").append(description).append("</title></head><body><center><h1>")\
        .append(error).append(" ").append(description).append("</h1></center></body></html>");
	
    data = "HTTP/1.1 " + error + " " + description + "\r\n";
    
    data.append("Content-Type: text/html\r\n").append("Content-Length: ").append(std::to_string(temp_msg_body.length())).append("\r\n").append("\r\n");
    data.append(temp_msg_body);
    data.append("\r\n");
    return (data);
}

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
    return ("text/plain");
}

std::string Response::readFile(const std::string& filename, Server &server) {
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
    return (parse_error_pages("404", "File not found.", server));
}

Path Response::find_default_index(Path &abs_path, Location *location) {
    abs_path = Path(abs_path.getPath(), DIRECTORY);
    // Check for location default indexes
    if (location->index.size() != 0) {
        std::vector<std::string>::iterator it = location->index.begin();
        std::vector<std::string>::iterator ite = location->index.end();
        for (; it != ite; it++) {
            Path index(*it, URI);
            Path temp_index = abs_path.concat(index.getPath(), URI);
            if (Path::isAccessible(temp_index.getPath().c_str())) {
                // If index is found, then break and readFile() below
                return (temp_index);
            }
        }
    }
    return (abs_path);
}

bool Response::isStaticContent(Location *location) {
	return (location->cgi_pass.getPath() == "");
}

// IF static content, then just return string using readFile
// TODO: implement default index
// TODO: implement directory listing
std::string Response::handleStaticContent(const Request& request, Location *location, Server &server) { // add argument location (so that I can use prepend root)
    // Handle static content based on the requested path.
    std::cout << "Handling static content..." << std::endl;

    try {
        Path request_uri = request.getPath();
        Path abs_path = Path::mapURLToFS(request_uri, location->uri, location->root);
        std::cout << "ABSOLUTE FILE PATH FORM LOCATION IS: " << abs_path.getPath() << std::endl;
        
        // Check if it's directory. Cannot use Path.type because mapURLtoFS defaults to URI type
        // TODO: Ask XueRui identify DIRECTORY type when mapURLtoFS
        if (abs_path.getPath()[abs_path.getPath().length() - 1] == '/') {
            abs_path = find_default_index(abs_path, location);

            // If still cannot find default index, then list directory
            if (abs_path.getType() == DIRECTORY) {
                if (location->autoindex == true) {
                    // TODO: list directories
                    std::cout << "AUTOINDEX IS: " << (location->autoindex ? "ON" : "OFF") << std::endl;
                } else {
                    return (parse_error_pages("403", "Forbidden", server));
                }
            }
        }
        return (readFile(abs_path.getPath(), server));
    } catch (Path::InvalidPathException &err) {
        return (parse_error_pages("501", err.what(), server));
    }
}

std::string Response::handle_GET_request(Request &request, Location *location, Server &server) {
    (void)request;
    (void)location;
    (void)server;
    return std::string();
}

std::string Response::handle_POST_request(Request &request, Location *location, Server &server) {
    if (location->cgi_pass.getPath() == "")
        return (parse_error_pages("405", "Method not allowed ", server));
    else if (request.getHeader("Content-Length") != "" && location->max_client_body_size != 0 && // CHECK: max_client_body_size when not defined?
    std::stoull(request.getHeader("Content-Length")) > location->max_client_body_size)
        return (parse_error_pages("413", "Payload Too Large", server));
    else {
        // CgiHandler cgi;
        // cgi.handle_cgi(request, *this, server, *location);
    }

    return std::string();
}

std::string Response::handle_DELETE_request(Request &request, Location *location, Server &server) {
    (void)request;
    (void)location;
    (void)server;
    return std::string();
}

Server &Response::find_server(Request& request, std::map<int, Server>& servers) {
    std::map<int, Server>::iterator it = servers.find(std::stoi(request.getPort())); // confirm what map<int> represents, should be port number

    if (it != servers.end()) {
        // Matching port found
        const Server& currentServer = it->second;

        if (std::find(currentServer.server_name.begin(), currentServer.server_name.end(), request.getHost()) != currentServer.server_name.end()) { // e.g. example.com == example.com?
            // Matching server found
            return const_cast<Server&>(currentServer);
        }
    }
    // If no exact match is found, return the server with the same index as the first server checked
    return (servers.begin()->second);
}

// START HERE - MAIN RESPONSE FUNCTION
std::string Response::generateResponse(Request &request, std::map<int, Server> &servers) {

    // Find the correct server based the host and port e.g. 192.168.0.1:8080
    Server server = find_server(request, servers);
    std::string method = request.getMethod();
    Path path = request.getPath();

    if (method != "GET" && method != "POST" && method != "DELETE")
        return (parse_error_pages("501", "Method not implemented", server));

    // PROCESSING RESPONSE
    // FIND the correct location block
    // e.g. _path = HTTP/1.1 /images/1.png GET
    Location *location = Path::getBestFitLocation(server.locations, path); // relative path
    
    if (location == NULL)
        return (parse_error_pages("403", "Forbidden", server));
    
    // if ((*location).get_return() != "") 
			// this->handle_return((*location).get_return());

    // Check if the resource is a static file
    if (isStaticContent(location)) {
        if (request.getMethod() == "GET")
            return (handleStaticContent(request, location, server));
        else
            return (parse_error_pages("405", "Method not allowed", server));
    }
    else {
        // TODO - Dynamic requests
        // Proceed with handling dynamic content with CGI
        if (request.getMethod() == "GET")
            return (handle_GET_request(request, location, server));
        else if (request.getMethod() == "POST")
            return (handle_POST_request(request, location, server));
        else if (request.getMethod() == "DELETE")
            return (handle_DELETE_request(request, location, server));
    }

    return std::string();
}
