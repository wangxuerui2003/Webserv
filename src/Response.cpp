/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 18:30:17 by zwong             #+#    #+#             */
/*   Updated: 2024/01/23 17:49:06 by zwong            ###   ########.fr       */
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
	return (temp_msg_body);
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
std::string Response::handleStaticContent(Path &absPath, Location *location, Server &server) { // add argument location (so that I can use prepend root)
    // Handle static content based on the requested path.
    std::cout << "Handling static content..." << std::endl;

    // Check if it's directory. Cannot use Path.type because mapURLtoFS defaults to URI type
    // TODO: Ask XueRui identify DIRECTORY type when mapURLtoFS
    if (absPath.getPath()[absPath.getPath().length() - 1] == '/') {
        absPath = find_default_index(absPath, location);

        // If still cannot find default index, then list directory
        if (absPath.getType() == DIRECTORY) {
            if (location->autoindex == true) {
                // TODO: list directories
                std::cout << "AUTOINDEX IS: " << (location->autoindex ? "ON" : "OFF") << std::endl;
            } else {
                return (parse_error_pages("403", "Forbidden", server));
            }
        }
    }
    return (readFile(absPath.getPath(), server));
}

std::string Response::handle_GET_request(Request &request, Location *location, Server &server) {
    (void)request;
    (void)location;
    (void)server;
    // CgiHandler cgi;
    // cgi.handle_cgi(request, *this, server, *location);
    return (std::string("WORK IN PROGRESS"));
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
        return (std::string("WORK IN PROGRESS"));
    }
}

std::string Response::deleteResource(Path &absPath, Server &server) {
    if (remove(absPath.getPath().c_str()) == -1) {
		if (errno == ENOENT) 
			return (parse_error_pages("404", "Not Found", server));
		else
			return (parse_error_pages("500", "Internal Server Error", server));
	}
	else {
        // hard coded success page
        std::string data;
		std::string response_content = "<html>\n<head><title>200 OK</title></head>\n<body>\n<center><h1>200 OK</h1></center>\n</body>\n</html>";
		data += "HTTP/1.1 200 OK\r\n";
		data += "Content-Type: text/html\r\n";
		data += "Content-Length: " + std::to_string(response_content.length()) + "\r\n";
		data += "\r\n";
		data += response_content;
		data += "\r\n";
        return (data);
	}
}

std::string Response::handle_DELETE_request(Path &absPath, Location *location, Server &server) {
    std::vector<std::string>::iterator head = location->allowedHttpMethods.begin();
    std::vector<std::string>::iterator end = location->allowedHttpMethods.end();
    if (std::find(head, end, "DELETE") == end) // if couldn't find DELETE, reached the end
        return (parse_error_pages("405", "Method not allowed", server));
    else
        return (deleteResource(absPath, server));
}

// TODO: Check with XueRui on config checks host and ports
Server &Response::findServer(Request &request, std::map<int, Server> &servers) {
    for (std::map<int, Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
        Server& currentServer = it->second;

        // Check hosts
        for (size_t j = 0; j < currentServer.hosts.size(); ++j) {
            std::pair<std::string, std::string> &hostPair = currentServer.hosts[j];
            if (request.getHost() == hostPair.first && request.getPort() == hostPair.second) {
                return (const_cast<Server&>(currentServer));
            }
        }

        // Check server names (no check ports)
        for (size_t k = 0; k < currentServer.server_name.size(); ++k) {
            const std::string& serverName = currentServer.server_name[k];
            if (request.getHost() == serverName) {
                return (const_cast<Server&>(currentServer));
            }
        }
    }

    // If no exact match is found, return the first server
    return const_cast<Server&>(servers.begin()->second);
}

// START HERE - MAIN RESPONSE FUNCTION
std::string Response::generateResponse(Request &request, std::map<int, Server> &servers) {

    // Find the correct server based the host and port e.g. 192.168.0.1:8080
    Server server = findServer(request, servers);
    std::string method = request.getMethod();
    Path request_uri = request.getPath();

    if (method != "GET" && method != "POST" && method != "DELETE")
        return (parse_error_pages("501", "Method not implemented", server));

    // PROCESSING RESPONSE
    // FIND the correct location block
    // e.g. _path = HTTP/1.1 /images/1.png GET
    Location *location = Path::getBestFitLocation(server.locations, request_uri); // relative path
    
    if (location == NULL)
        return (parse_error_pages("403", "Forbidden", server));
    
    // if ((*location).get_return() != "") 
			// this->handle_return((*location).get_return());

    // Get absolute resource path
    Path absPath;
    try {
        absPath = Path::mapURLToFS(request_uri, location->uri, location->root);
        std::cout << "ABSOLUTE FILE PATH FORM LOCATION IS: " << absPath.getPath() << std::endl;
    } catch (Path::InvalidOperationException &err) {
        return (parse_error_pages("501", err.what(), server));
    }

    // Check if the resource is a static file
    if (isStaticContent(location)) {
        if (request.getMethod() == "GET")
            return (handleStaticContent(absPath, location, server));
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
            return (handle_DELETE_request(absPath, location, server));
    }

    return std::string();
}
