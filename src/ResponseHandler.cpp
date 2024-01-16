/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHandler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 18:30:17 by zwong             #+#    #+#             */
/*   Updated: 2024/01/16 16:21:30 by zwong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseHandler.hpp"

// TODO: Decide how to check if request is dynamic or static (based on file extension?)
bool ResponseHandler::isCgiRequest(const std::string& path) {
    (void)path;
    // Check if the request path corresponds to CGI scripts.

    // Example: Check if the path starts with "/cgi-bin/"
    return (false);
}

std::string ResponseHandler::getContentType(const std::string& filePath) {
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

std::string ResponseHandler::readFile(const std::string& filename) {
    std::string response;
    std::ifstream file(filename);
    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        response += "HTTP/1.1 200 OK\r\n\r\n";
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
std::string ResponseHandler::handleStaticContent(const Request& httpRequest) {
    // Handle static content based on the requested path.
    std::cout << "Handling static content..." << std::endl;

    std::string path = "www/" + httpRequest.getPath().substr(1);  // Remove the leading '/' from the path.
    std::cout << "FILE PATH: " << path << std::endl;
    // You may implement more sophisticated logic here, such as serving files based on the path.
    return (readFile(path));
}

std::string ResponseHandler::generateResponse(const std::string& rawReqString) {
    // Parse raw request string into object
    const Request httpRequest(rawReqString);

    std::string method = httpRequest.getMethod();
    std::string path = httpRequest.getPath();
    std::string version = httpRequest.getVersion();

    if (method != "GET" && method != "POST" && method != "DELETE")
        return ("HTTP/1.1 501 Not Implemented\r\n\r\nMethod not implemented.");

    if (isCgiRequest(path)) {
        // TODO: Create CGIHandler class
        // return CgiHandler::handleCgiRequest(httpRequest);
        return ("");
    } else {
        // WORKING ON STATIC CONTENT FIRST
        return handleStaticContent(httpRequest);
    }
}