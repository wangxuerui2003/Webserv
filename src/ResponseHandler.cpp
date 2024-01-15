/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHandler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 18:30:17 by zwong             #+#    #+#             */
/*   Updated: 2024/01/15 18:45:00 by zwong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseHandler.hpp"

// TODO: Decide how to check if request is dynamic or static (based on file extension?)
bool ResponseHandler::isCgiRequest(const std::string& path) {
    // Check if the request path corresponds to CGI scripts.
    // You may implement this based on your server's CGI handling criteria.
    // For example, checking if the path starts with "/cgi-bin/" or has a specific file extension.
    // Modify this function according to your project requirements.

    // Example: Check if the path starts with "/cgi-bin/"
    return (path.substr(0, 9) == "/cgi-bin/");
}

// IF static content, then just return string using readFile
std::string ResponseHandler::handleStaticContent(const Request& httpRequest) {
    // Handle static content based on the requested path.
    // This is a placeholder; replace it with your actual static content handling logic.

    std::string path = httpRequest.getPath().substr(1);  // Remove the leading '/' from the path.

    // You may implement more sophisticated logic here, such as serving files based on the path.
    return readFile(path);
}

std::string ResponseHandler::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        return "HTTP/1.1 200 OK\r\n\r\n" + buffer.str();
    }
    return "HTTP/1.1 404 Not Found\r\n\r\nFile not found.";
}

std::string ResponseHandler::generateResponse(const std::string& rawReqString) {
    // Parse raw request string into object
    const Request httpRequest(rawReqString);

    std::string method = httpRequest.getMethod();
    std::string path = httpRequest.getPath();
    std::string version = httpRequest.getVersion();

    if (method != "GET" && method != "POST" && method != "DELETE") {
        if (isCgiRequest(path)) {
            // Handle CGI request
            // TODO: Create CGIHandler class
            // return CgiHandler::handleCgiRequest(httpRequest);
        } else {
            // Handle static content
            return handleStaticContent(httpRequest);
        }
    } else {
        // Respond with a simple error message for unsupported methods.
        return "HTTP/1.1 501 Not Implemented\r\n\r\nMethod not implemented.";
    }
}