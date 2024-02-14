/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 17:48:35 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/14 19:28:07 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

void Parser::parseEventContext(Config& config, std::vector<std::string>& configLines, size_t& currentLineIndex) {
    std::vector<std::string> eventLines;
    while (++currentLineIndex < configLines.size()) {
        if (configLines[currentLineIndex].find("}") != std::string::npos) {
            ++currentLineIndex;
            break;
        }
        eventLines.push_back(configLines[currentLineIndex]);
    }

    std::vector<std::string> eventHandlerVect = getKeywordValues("event_handler", eventLines);
    if (eventHandlerVect.size() > 1) {
        wsutils::errorExit("Too many event handlers");
    } else if (eventHandlerVect.empty()) {
        wsutils::errorExit("No event handler specified in event context");
    }

    if (eventHandlerVect[0] == "select") {
        config.eventHandlerType = SELECT;
    } else if (eventHandlerVect[0] == "poll") {
        config.eventHandlerType = POLL;
    } else {
        wsutils::errorExit("Invalid event handler");
    }
}

Location Parser::parseLocationContext(std::vector<std::string>& serverLines, size_t& currentLineIndex) {
    std::vector<std::string> locationLines;
    while (currentLineIndex < serverLines.size()) {
        if (serverLines[currentLineIndex].find("}") != std::string::npos) {
            ++currentLineIndex;
            break;
        }
        locationLines.push_back(serverLines[currentLineIndex]);
        currentLineIndex++;
    }

    Location location;
    location.uri = Path(getKeywordValues("location", locationLines)[0], URI);
    
    // detect HTTP redirection
    std::vector<std::string> httpRedirection = getKeywordValues("return", locationLines);
    if (httpRedirection.empty() == false) {
        if (httpRedirection.size() != 2) {
            wsutils::errorExit("Http Redirection format should be: return <status code> <url>");
        }
        location.isHttpRedirection = true;
        location.redirectionStatusCode = httpRedirection[0];
        location.redirectURL = httpRedirection[1];
        return location;  // ignore everything else in the block when it's a http redirection
    }

    // If location doesn't have root, get from server block
    std::vector<std::string> roots = getKeywordValues("root", locationLines);
    if (roots.empty()) {
        location.root = Path(getKeywordValues("root", serverLines)[0]);
        location.isCustomRoot = false;
    } else {
        location.root = Path(getKeywordValues("root", locationLines)[0]);
        if (location.root.getPath()[0] != '/') {
            throw Path::InvalidPathException("root directives should be in Absolute Path format");
        }
        location.isCustomRoot = true;
    }
    
    // If location doesn't have index vector, get from server block
    std::vector<std::string> index = getKeywordValues("index", locationLines);
    location.index = getKeywordValues("index", !index.empty() ? locationLines : serverLines);

    // Autoindex
    std::vector<std::string> autoindex = getKeywordValues("autoindex", locationLines);
    location.autoindex = autoindex.empty() ? false : getKeywordValues("autoindex", locationLines)[0] == "on" ? true : false;

    // client_max_body_size
    std::vector<std::string> mcbsStrs = getKeywordValues("client_max_body_size", locationLines);
    location.max_client_body_size = mcbsStrs.empty() ? 0 : wsutils::convertSizeStringToBytes(mcbsStrs[0]);

    // upload_store
    std::vector<std::string> uploadStoreStrs = getKeywordValues("upload_store", locationLines);
    if (uploadStoreStrs.empty() == false) {
        location.upload_store = Path(uploadStoreStrs[0], IGNORE);
        location.accept_upload = true;
    } else {
        location.accept_upload = false;
    }

    // limit_except (allowed HTTP methods)
    std::vector<std::string> allowedHttpMethods = getKeywordValues("limit_except", locationLines);
    if (allowedHttpMethods.empty() == false) {
        location.allowedHttpMethods = allowedHttpMethods;
    }

    return location;
}

void Parser::parseServerContext(Config& config, std::vector<std::string>& configLines, size_t& currentLineIndex) {
    std::vector<std::string> serverLines;
    std::vector<Location> locations;
    while (++currentLineIndex < configLines.size()) {
        if (configLines[currentLineIndex].find("location") != std::string::npos) {
            locations.push_back(parseLocationContext(configLines, currentLineIndex));
        }
        if (configLines[currentLineIndex].find("}") != std::string::npos) {
            ++currentLineIndex;
            break;
        }

        serverLines.push_back(configLines[currentLineIndex]);
    }

    Server server;

    std::vector<std::string> listens = getKeywordValues("listen", serverLines);
    for (std::vector<std::string>::iterator listen = listens.begin(); listen != listens.end(); ++listen) {
        std::string host, port;
        // listen can be host:port / port
        if (listen->find(':') == std::string::npos) {
            port = *listen;
            host = "0.0.0.0";
        } else {
            std::istringstream iss(*listen);
            std::getline(iss, host, ':');
            std::getline(iss, port);
        }
        std::pair<std::string, std::string> currHostPair = std::make_pair(host, port);
        std::vector<std::pair<std::string, std::string> >::iterator hostPair;
        for (hostPair = server.hosts.begin(); hostPair != server.hosts.end(); ++hostPair) {
            if (currHostPair.first == hostPair->first && currHostPair.second == hostPair->second) {
                throw Path::InvalidPathException("duplicate listen in server block");
            }
        }
        
        server.hosts.push_back(currHostPair);
    }
    if (!getKeywordValues("root", serverLines).empty()) {
        server.root = Path(getKeywordValues("root", serverLines)[0], DIRECTORY);
        if (server.root.getPath()[0] != '/') {
            throw Path::InvalidPathException("root directives should be in Absolute Path format");
        }
    } else {
        throw Path::InvalidPathException("No root in server block");
    }

    server.index = getKeywordValues("index", serverLines);
    if (server.index.empty()) {
        wsutils::errorExit("Server no index");
    }

    server.server_name = getKeywordValues("server_name", serverLines);

    std::vector<std::string> cgi_handlers = getKeywordValues("cgi_handler", serverLines);
    for (size_t cgiIndex = 0; cgiIndex < cgi_handlers.size(); ++cgiIndex) {
        if (cgiIndex + 1 == cgi_handlers.size()) {
            server.cgiHandlers[cgi_handlers[cgiIndex]] = "";
        } else if (cgi_handlers[cgiIndex + 1][0] == '.' && cgi_handlers[cgiIndex + 1][1] != '/') {
            // The next string is a file extension, means current cgi has no handler
            server.cgiHandlers[cgi_handlers[cgiIndex]] = "";
        } else {
            Path handler(cgi_handlers[cgiIndex + 1]);
            if (handler.isExecutable() == false) {
                throw Path::InvalidPathException(cgi_handlers[cgiIndex + 1] + " is not executable");
            }
            server.cgiHandlers[cgi_handlers[cgiIndex]] = handler.getPath();
            cgiIndex++;
        }
    }

    // [404, 404.html, 501, 501.html]
    std::vector<std::string> errorPages = getKeywordValues("error_page", serverLines);
    if (errorPages.size() % 2 != 0) {
        wsutils::errorExit("Error Pages should be key-value pairs!");
    }
    // [404, 404.html] => 404 => root/404.html
    for (std::vector<std::string>::iterator it = errorPages.begin(); it != errorPages.end(); it += 2) {
        Path errorPagePath = server.root.concat(*(it + 1), URI);
        server.error_pages[*it] = errorPagePath;
    }

    std::vector<std::string> sessionStore = getKeywordValues("session_store", serverLines);
    if (sessionStore.empty() == false) {
        server.hasSessionManagement = true;
        server.session = Session(sessionStore[0]);
        std::vector<std::string> session_expire_seconds = getKeywordValues("session_expire_seconds", serverLines);
        if (session_expire_seconds.empty()) {
            server.sessionExpireSeconds = 60;
        } else {
            server.sessionExpireSeconds = wsutils::stringToNumber<time_t>(session_expire_seconds[0]);
        }
    }

    server.locations = locations;

    config.servers.push_back(server);
}


void Parser::parse(std::string configFilePath, Config& config) {
    Path configPath(configFilePath);
    std::vector<std::string> configLines = configPath.readLines();

    for (size_t currentLineIndex = 0;
        currentLineIndex < configLines.size(); ++currentLineIndex) {
        if (configLines[currentLineIndex].find("event {") != std::string::npos) {
            parseEventContext(config, configLines, currentLineIndex);
        }

        if (configLines[currentLineIndex].find("server {") != std::string::npos) {
            parseServerContext(config, configLines, currentLineIndex);
            std::cout << "here" << std::endl;
        }
    }
}

void    Parser::getLineValues(std::vector<std::string>& values, std::string& keyword, std::string& line) {
    std::string value;
    std::istringstream lineStream(line);
    lineStream >> value;

    if (value != keyword) {
        return;
    }

    while (!lineStream.eof()) {
        lineStream >> value;
        if (value == ";") {
            continue;
        }
        if (lineStream.eof()
            && (value[value.length() - 1] == ';' || value[value.length() - 1] == '{')) {
            values.push_back(value.substr(0, value.size() - 1));
        } else {
            values.push_back(value);
        }
    }
}

std::vector<std::string> Parser::getKeywordValues(std::string keyword, std::vector<std::string>& lines) {
    std::vector<std::string> values;
    
    for (size_t lineIndex = 0; lineIndex < lines.size(); ++lineIndex) {
        if (lines[lineIndex].find(keyword) != std::string::npos) {
            getLineValues(values, keyword, lines[lineIndex]);
        }
    }

    return values;
}

void Parser::print_server(const Server &server) {
    std::cout << GREEN;
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = server.hosts.begin(); it != server.hosts.end(); ++it)
        std::cout << "IP Address: " << it->first << " | " << "Port: " << it->second << std::endl;

    for (std::vector<std::string>::const_iterator it = server.server_name.begin(); it != server.server_name.end(); ++it)
        std::cout << "Server Name: " << *it << " " << std::endl;

    for (std::vector<std::string>::const_iterator it = server.index.begin(); it != server.index.end(); ++it)
        std::cout << "Index: " << *it << " " << std::endl;

    if (!server.root.getPath().empty())
        std::cout << "Root: " << server.root.getPath() << std::endl;

    for (std::map<std::string, Path>::const_iterator it = server.error_pages.begin(); it != server.error_pages.end(); ++it)
        std::cout << "Error Number: " << it->first << " | " << "Path: " << it->second.getPath() << std::endl;

    if (server.hasSessionManagement) {
        std::cout << "Session Store: " << server.session.getSessionStorePath().getPath() << std::endl;
        std::cout << "Session Expire Seconds: " << server.sessionExpireSeconds << std::endl;
    }

    std::cout << RESET;
}

void Parser::print_location(const Location &location) {
    std::cout << BLUE;
    if (!location.uri.getPath().empty())
        std::cout << "URI: " << location.uri.getPath() << std::endl;
    if (!location.root.getPath().empty())
        std::cout << "Root: " << location.root.getPath() << std::endl;
    // if (!location.cgi_pass.getPath().empty())
    //     std::cout << "CGI Pass: " << location.cgi_pass.getPath() << std::endl;
    for (std::vector<std::string>::const_iterator it = location.index.begin(); it != location.index.end(); ++it)
        std::cout << "Index: " << *it << " " << std::endl;
    std::cout << "Auto Index: " << std::boolalpha << location.autoindex << std::endl;
    std::cout << "Max Client Body Size: " << location.max_client_body_size << std::endl;
    if (!location.upload_store.getPath().empty())
        std::cout << "Upload Store: " << location.upload_store.getPath() << std::endl;
    std::cout << "Accept Upload: " << std::boolalpha << location.accept_upload << std::endl;
    for (std::vector<std::string>::const_iterator it = location.allowedHttpMethods.begin(); it != location.allowedHttpMethods.end(); ++it)
        std::cout << "allowedHttpMethods: " << *it << " " << std::endl;
    std::cout << RESET;
}

void Parser::print_values(std::vector<Server>& _servers) {
    int server_index = 0;
    for (std::vector<Server>::iterator server = _servers.begin(); server != _servers.end(); server++) {
        server_index++;
        std::cout << BOLD << GREEN << "\nServer " << server_index << RESET << std::endl;
        print_server(*server);
        int location_index = 0;
        for (std::vector<Location>::iterator location = server->locations.begin(); location != server->locations.end(); location++) {
            location_index++;
            std::cout << BOLD << BLUE << "\nLocation " << location_index << RESET << std::endl;
            print_location(*location);
        }
    }
}
