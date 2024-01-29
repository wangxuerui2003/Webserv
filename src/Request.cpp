/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:21:52 by zwong             #+#    #+#             */
/*   Updated: 2024/01/29 17:50:19 by zwong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(const std::string& rawReqString) {
    parseRequest(rawReqString);
}

std::string Request::getMethod() const {
    return (_method);
}

Path Request::getPath() const {
    return (_path);
}

std::string Request::getVersion() const {
    return (_httpVersion);
}

std::string Request::getHost() const {
    return (_host);
}

std::string Request::getPort() const {
    return (_port);
}

std::string Request::getHeader(const std::string& headerName) const {
    // Need to copu into a temp because _headers is intepreted as const_iterator?
    std::map<std::string, std::string> temp = _headers;
    for (std::map<std::string, std::string>::iterator head = temp.begin(); head != temp.end(); ++head) {
        if (head->first == headerName) {
            return (head->second);
            break; // Exit the loop once the header is found
        }
    }
    return (""); // When header not found
}

std::map<std::string, std::string> &Request::getHeaderMap() {
    return (_headers);
}

std::string Request::getBody() const {
    return (_body);
}

// example of raw string is:
// GET /foo/bar HTTP/1.1
// Host: example.org
// User-Agent: Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10.6; fr; rv:1.9.2.8) Gecko/20100722 Firefox/3.6.8
// Accept: */*
// Accept-Language: fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3
// Accept-Encoding: gzip,deflates
// Accept-Charset: I
void Request::parseRequest(const std::string& rawReqString) {
    std::istringstream requestStream(rawReqString);
    std::string temp_path;
    requestStream >> _method >> temp_path >> _httpVersion; // extraction operator splits by whitespaces

    _path = Path(temp_path, URI);

    // Read headers until an empty line is encountered
    std::string line;
    std::getline(requestStream, line);  // Skips the method, path, version -> go to headers
    while (std::getline(requestStream, line) && !line.empty()) {
        parseHeaders(line);
    }

    // Read the request body if present
    std::getline(requestStream, _body);
}

void Request::parseHeaders(const std::string& headerPart) {
    std::istringstream headerStream(headerPart);
    std::string line;

    while (std::getline(headerStream, line)) {
        if (line.back() == '\r') {
            line.pop_back();
        }
        size_t colonPos = line.find(':');

        if (colonPos != std::string::npos) {
            std::string headerName = line.substr(0, colonPos);
            std::string headerValue = line.substr(colonPos + 2);

            // Extract Host and Port variable
            if (headerName == "Host") {
                size_t hostColonPos = headerValue.find(':');
                if (hostColonPos != std::string::npos) {
                    this->_host = headerValue.substr(0, hostColonPos);
                    this->_port = headerValue.substr(hostColonPos + 1);
                } else {
                    this->_host = headerValue;
                }
            }
            
            // Trim leading and trailing whitespaces from headerValue
            headerValue.erase(0, headerValue.find_first_not_of(" \t"));
            headerValue.erase(headerValue.find_last_not_of(" \t") + 1);

            _headers.insert(std::make_pair(headerName, headerValue));
        }
    }
}

// void Request::parseHeaders(const std::string& headerPart) {
//     std::istringstream headerStream(headerPart);
//     std::string headerName, headerValue;
//     std::getline(headerStream, headerName, ':');
//     std::getline(headerStream, headerValue);
//     _headers.insert(std::make_pair(headerName, headerValue));
// }
