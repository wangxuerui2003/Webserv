/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 18:30:17 by zwong             #+#    #+#             */
/*   Updated: 2024/02/06 12:43:27 by wxuerui          ###   ########.fr       */
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
    mimeTypes["htm"] = "text/html";
    mimeTypes["css"] = "text/css";
    mimeTypes["js"] = "application/javascript";
    mimeTypes["ico"] = "image/x-icon";
    mimeTypes["png"] = "image/png";
    mimeTypes["gif"] = "image/gif";
    mimeTypes["jpg"] = "image/jpeg";
    mimeTypes["jpeg"] = "image/jpeg";
    mimeTypes["bmp"] = "image/bmp";
    mimeTypes["svg"] = "image/svg+xml";
    mimeTypes["pdf"] = "application/pdf";
    mimeTypes["txt"] = "text/plain";
    mimeTypes["xml"] = "application/xml";
    mimeTypes["json"] = "application/json";
    mimeTypes["zip"] = "application/zip";
    mimeTypes["tar"] = "application/x-tar";
    mimeTypes["gz"] = "application/gzip";
    mimeTypes["mp3"] = "audio/mpeg";
    mimeTypes["wav"] = "audio/wav";
    mimeTypes["mp4"] = "video/mp4";
    mimeTypes["avi"] = "video/x-msvideo";
    
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
    if (abs_path.getType() != DIRECTORY) {
        return abs_path;
    }

    // Check for location default indexes specified in the location config block
    if (location->index.size() != 0) {
        std::vector<std::string>::iterator it = location->index.begin();
        std::vector<std::string>::iterator ite = location->index.end();
        for (; it != ite; it++) {
            Path index(*it, URI);
            try {
                Path temp_index = abs_path.concat(index.getPath());
                return (temp_index);
            } catch (...) {
                // Continue looping
            }
        }
    }
    return (abs_path);
}

bool Response::isStaticContent(const Path& uri, Server& server) {
    std::map<std::string, std::string>::iterator it = server.cgiHandlers.begin();
    for (; it != server.cgiHandlers.end(); ++it) {
        if (it->first == uri.getFileExtension()) {
            return false;
        }
    }

    return true;
}

// Handle static content based on the requested absoulte path
std::string Response::handleStaticContent(Request& request, Path &absPath, Location *location, Server &server) {
    if (absPath.getType() == DIRECTORY) {
        
        Path index = find_default_index(absPath, location);

        if (index != absPath) {
            return (readFile(index, server));
        }

        // If still cannot find default index, then list directory
        if (location->autoindex == true) {
            return (generateDirectoryListing(absPath, request.getURI()));
        } else {
            return (parse_error_pages("403", "Forbidden", server));
        }
    }
    return (readFile(absPath, server));
}

std::string Response::handle_GET_request(Request &request, Location *location, Server &server) {
    return (CgiHandler::handleCgi(request, server, *location));
}

std::string Response::handle_POST_request(Request &request, Location *location, Server &server) {
    if (isStaticContent(request.getURI(), server)) {
        return (parse_error_pages("405", "Method not allowed ", server));
    }
    else if (request.getHeader("Content-Length") != "" && location->max_client_body_size != 0 && // CHECK: max_client_body_size when not defined?
    wsutils::stringToNumber<unsigned long long>(request.getHeader("Content-Length")) > location->max_client_body_size)
        return (parse_error_pages("413", "Payload Too Large", server));
    else {
        return (CgiHandler::handleCgi(request, server, *location));
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

// std::string Response::handle_DELETE_request(Path &absPath, Location *location, Server &server) {
//     std::vector<std::string>::iterator head = location->allowedHttpMethods.begin();
//     std::vector<std::string>::iterator end = location->allowedHttpMethods.end();
//     if (std::find(head, end, "DELETE") == end) // if couldn't find DELETE, reached the end
//         return (parse_error_pages("405", "Method not allowed", server));
//     else
//         return (deleteResource(absPath, server));
// }

Server &Response::findServer(Request &request, std::map<int, Server*> &servers) {
    for (std::map<int, Server*>::iterator it = servers.begin(); it != servers.end(); ++it) {
        Server& currentServer = *(it->second);

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

    throw InvalidServerException();
}

std::string httpRedirection(std::string statusCode, std::string url) {
    std::string statusMessage;

    if (statusCode == "301") {
        statusMessage = "Moved Permanently";
    } else if (statusCode == "302") {
        statusMessage = "Found";
    } else if (statusCode == "303") {
        statusMessage = "See Other";
    } else if (statusCode == "307") {
        statusMessage = "Temporary Redirect";
    } else if (statusCode == "308") {
        statusMessage = "Permanent Redirect";
    } else {
        statusMessage = "Moved Permanently";
    }

    return (
        "HTTP/1.1 "
        + statusCode
        + ' '
        + statusMessage
        + "\r\nLocation: "
        + url
        + "\r\n"
        + "Content-Type: text/html; charset=utf-8\r\n"
        + "Content-Length: 0\r\n"
        + "Connection: close\r\n"
        + "\r\n"
    );
}

// START HERE - MAIN RESPONSE FUNCTION
std::string Response::generateResponse(Request &request, Server &server) {
    std::string method = request.getMethod();
    Path request_uri = request.getURI();

    // Projected is lmited to 3 http methods only (further lmitation is handled later on)
    if (method != "GET" && method != "POST" && method != "DELETE") {
        return (parse_error_pages("405", "Method Not Allowed", server));
    }
    
    // Finding the correct location block from the short URI given (crosshecks config file)
    Location *location = getBestFitLocation(server.locations, request_uri);
    
    // If NULL, means no such path found
    if (location == NULL)
        return (parse_error_pages("403", "Forbidden", server));

    // Found matching location block, then chekc the limited METHODS
    if (std::find(location->allowedHttpMethods.begin(), location->allowedHttpMethods.end(), method) == location->allowedHttpMethods.end()) {
        return (parse_error_pages("405", "Method not allowed", server));
    }
    
    if (location->isHttpRedirection) {
        return httpRedirection(location->redirectionStatusCode, location->redirectURL);
    }

    // Once server and location is matched, get absolute resource path
    Path absPath;
    try {
        // Map URL to filesystem
        absPath = Path::mapURLToFS(request_uri, location->uri, location->root, location->isCustomRoot);
    } catch (Path::InvalidOperationException &err) {
        return (parse_error_pages("500", err.what(), server));
    } catch (Path::InvalidPathException &err) {
        return (parse_error_pages("404", err.what(), server));
    }


    if (absPath.getType() == DIRECTORY) {
        absPath = find_default_index(absPath, location);
    }

    // Check if the resource is a STATIC file by file extentions (e.g. .html, .py, .png)
    if (isStaticContent(absPath, server)) {
        // Only GET method is alloed for static content
        if (request.getMethod() == "GET")
            return (handleStaticContent(request, absPath, location, server));
        else if (request.getMethod() == "DELETE")
            return (deleteResource(absPath, server));
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
    }
    return (parse_error_pages("400", "Bad Request", server));
}

const char *Response::InvalidServerException::InvalidServerException::what() const throw() {
    return ("Invalid server");
}

/**
 * @param locations: a vector of location patterns/ uri prefix
 * @param requestUri: the request uri
 * 
 * @brief Get the best fit location that the request URI is requesting for.
 * 
*/
Location *Response::getBestFitLocation(std::vector<Location>& locations, Path& requestUri) {
	Location *bestFit = NULL;
	size_t layersMatched = 0;

	for (size_t i = 0; i < locations.size(); ++i) {
		const std::string& uriRef = requestUri.getPath();
		const std::string& uriPrefixRef = locations[i].uri.getPath();
		size_t start = 0;
		size_t layersMatchedTemp = 0;
		
		while (start < uriPrefixRef.length()) {
			size_t uriDelimiter = uriRef.find('/', start);
			if (uriDelimiter == std::string::npos) {
				uriDelimiter = uriRef.length();
			}
			
			size_t uriPrefixDelimiter = uriPrefixRef.find('/', start);
			if (uriPrefixDelimiter == std::string::npos) {
				uriPrefixDelimiter = uriPrefixRef.length();
			}

			// The position of the next / doesn't match, wrong location
			if (uriDelimiter != uriPrefixDelimiter) {
				layersMatchedTemp = 0;
				break;
			}

			// The segment of uri and uriPrefix doesn't match, wrong location
			if (uriRef.substr(start, uriDelimiter - start) != uriPrefixRef.substr(start, uriPrefixDelimiter - start)) {
				layersMatchedTemp = 0;
				break;
			}

			start = uriDelimiter + 1;
			layersMatchedTemp++;
		}

		if (layersMatchedTemp > layersMatched) {
			layersMatched = layersMatchedTemp;
			bestFit = &(locations[i]);
		}
	}

	return bestFit;
}

std::string Response::generateDirectoryListing(const Path& dirPath, const Path& uri) {
	if (dirPath.getType() != DIRECTORY)
		throw Path::InvalidOperationException("File " + dirPath.getPath() + " is not a directory");
    std::string html = "<html>\n<head><title>Index of " + dirPath.getPath() + "</title></head>\n<body>\n<h1>Index of " + dirPath.getPath() + "</h1>\n<ul>\n";

    DIR* dir = opendir(dirPath.getPath().c_str());
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            std::string entryName = entry->d_name;
            html += "<li><a href=\"" + uri.concat(entryName, IGNORE).getPath() + "\">" + entryName + "</a></li>\n";
        }
        closedir(dir);
    }

    html += "</ul>\n</body>\n</html>\n";

	std::string response;

	response += "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/html\r\n";
	response += ("Content-Length: " + wsutils::toString<size_t>(html.length()) + "\r\n");
	response += "\r\n";
	response += html;
	return (response);
}
