/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 18:30:17 by zwong             #+#    #+#             */
/*   Updated: 2024/01/31 12:21:41 by zwong            ###   ########.fr       */
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
            temp_msg_body = head->second.read();
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
    
    data.append("Content-Type: text/html\r\n").append("Content-Length: ").append(wsutils::toString(temp_msg_body.length())).append("\r\n").append("\r\n");
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
    return ("text/plain");
}

std::string Response::readFile(Path &absPath, Server &server) {
    std::string response;
    if (Path::isAccessible(absPath.getPath().c_str())) {
        std::string content = absPath.read();
        response += "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: " + getContentType(absPath.getPath()) + "\r\n";
        response += ("Content-Length: " + wsutils::toString(content.length()) + "\r\n");
        response += "\r\n";
        response += content;
        return (response);
    }
    return (parse_error_pages("404", "File not found.", server));
}

Path Response::find_default_index(Path &abs_path, Location *location) {
    abs_path = Path(abs_path.getPath(), DIRECTORY);
    // Check for location default indexes specified in the location config block
    if (location->index.size() != 0) {
        std::vector<std::string>::iterator it = location->index.begin();
        std::vector<std::string>::iterator ite = location->index.end();
        for (; it != ite; it++) {
            Path index(*it, URI);
            Path temp_index = abs_path.concat(index.getPath(), URI);
            if (Path::isAccessible(temp_index.getPath().c_str())) {
                // If index is found, then break and readFile() in the main function
                return (temp_index);
            }
        }
    }
    return (abs_path);
}

bool Response::isStaticContent(Request& request, Server& server) {
    return find(server.cgi_extensions.begin(), server.cgi_extensions.end(), request.getPath().getFileExtension()) == server.cgi_extensions.end();
}

// Handle static content based on the requested absoulte path
std::string Response::handleStaticContent(Path &absPath, Location *location, Server &server) { // add argument location (so that I can use prepend root)
    wsutils::log("HANDLING STATIC REQUEST: " + absPath.getPath(), "./logs");

    // TODO: http://zwong.42.fr:8080/public/uploads/ | http://zwong.42.fr:8080/public/uploads (no slash won't work)
    if (absPath.getPath()[absPath.getPath().length() - 1] == '/') {
        absPath = find_default_index(absPath, location);

        // If still cannot find default index, then list directory
        if (absPath.getType() == DIRECTORY) {
            if (location->autoindex == true) {
                return (absPath.generateDirectoryListing());
            } else {
                return (parse_error_pages("403", "Forbidden", server));
            }
        }
    }
    return (readFile(absPath, server));
}

std::string Response::handle_GET_request(Request &request, Location *location, Server &server) {
    wsutils::log("GET: HANDLING CGI!", "./logs");
    CgiHandler cgi;
    return (cgi.handleCgi(request, server, *location));
}

std::string Response::handle_POST_request(Request &request, Location *location, Server &server) {
    if (find(server.cgi_extensions.begin(), server.cgi_extensions.end(), request.getPath().getFileExtension()) == server.cgi_extensions.end()) {
        return (parse_error_pages("405", "Method not allowed ", server));
    }
    else if (request.getHeader("Content-Length") != "" && location->max_client_body_size != 0 && // CHECK: max_client_body_size when not defined?
    wsutils::stringToNumber<unsigned long long>(request.getHeader("Content-Length")) > location->max_client_body_size)
        return (parse_error_pages("413", "Payload Too Large", server));
    else {
        wsutils::log("POST: HANDLING CGI!", "./logs");
        CgiHandler cgi;
        return (cgi.handleCgi(request, server, *location));
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
        // Return HTTP response
        std::string data;
		std::string response_content = "<html>\n<head><title>200 OK</title></head>\n<body>\n<center><h1>200 OK</h1></center>\n</body>\n</html>";
		data += "HTTP/1.1 200 OK\r\n";
		data += "Content-Type: text/html\r\n";
		data += "Content-Length: " + wsutils::toString(response_content.length()) + "\r\n";
		data += "\r\n";
		data += response_content;
		data += "\r\n";
        return (data);
	}
}

std::string Response::handle_DELETE_request(Path &absPath, Location *location, Server &server) {
    (void)location;
    return (deleteResource(absPath, server));
}

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

    // If no exact match is found
    wsutils::log("Invalid server host: " + request.getHost(), "./logs");
    throw InvalidServerException();
}

// START HERE - MAIN RESPONSE FUNCTION
std::string Response::generateResponse(Request &request, std::map<int, Server> &servers) {

    Server server;
    std::string method = request.getMethod();
    Path request_uri = request.getPath();
        
    try {
        // Find the correct server based the HOST and PORT and SERVER NAME (e.g. 127.0.0.1:8080)
        server = findServer(request, servers);
    } catch (InvalidServerException &err) {
        return (parse_error_pages("502", "Bad Server Gateway", server));
    }

    // Projected is lmited to 3 http methods only (further lmitation is handled later on)
    if (method != "GET" && method != "POST" && method != "DELETE") {
        return (parse_error_pages("501", "Method not implemented", server));
    }
    
    // Finding the correct location block from the short URI given (crosshecks config file)
    Location *location = Path::getBestFitLocation(server.locations, request_uri);
    
    // If NULL, means no such path found
    if (location == NULL)
        return (parse_error_pages("403", "Forbidden", server));

    // Found matching location block, then chekc the limited METHODS
    if (std::find(location->allowedHttpMethods.begin(), location->allowedHttpMethods.end(), method) == location->allowedHttpMethods.end()) {
        return (parse_error_pages("405", "Method not allowed", server));
    }

    // Once server and location is matched, get absolute resource path
    Path absPath;
    try {
        // Map URL to filesystem
        absPath = Path::mapURLToFS(request_uri, location->uri, location->root, location->isCustomRoot);
    } catch (Path::InvalidOperationException &err) {
        return (parse_error_pages("501", err.what(), server));
    }

    // Check if the resource is a STATIC file by file extentions (e.g. .html, .py, .png)
    if (isStaticContent(request, server)) {
        wsutils::log("Static Content", "./logs");
        // Only GET method is alloed for static content
        if (request.getMethod() == "GET")
            return (handleStaticContent(absPath, location, server));
        else
            return (parse_error_pages("405", "Method not allowed", server));
    }
    else {
        // Proceed with handling DYNAMIC content (e.g. upload, delete)
        if (!Path::isAccessible(absPath.getPath().c_str()))
			return (parse_error_pages("404", "Not Found", server));
        if (request.getMethod() == "GET")
            return (handle_GET_request(request, location, server));
        else if (request.getMethod() == "POST")
            return (handle_POST_request(request, location, server));
        else if (request.getMethod() == "DELETE")
            return (handle_DELETE_request(absPath, location, server));
    }
    return (parse_error_pages("400", "Bad Request", server));
}

const char *Response::InvalidServerException::InvalidServerException::what() const throw() {
    return ("Invalid server");
}
