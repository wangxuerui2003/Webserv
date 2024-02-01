/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 17:48:35 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/01 16:49:11 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser() {
}

Parser& Parser::getInstance() {
    static Parser instance;
	return instance;
}

const std::vector<Server>& Parser::getServers(void) const {
	return _servers;
}

void Parser::parse(std::string configFilePath) {
	// Path configFile(configFilePath, REG_FILE);

    std::ifstream configFile(configFilePath.c_str(), std::ios_base::in);
	if (!configFile.is_open()) {
        std::cerr << "Error opening config file." << std::endl;
        return ;
    }

    std::vector<std::string> configLines;
	std::string line;
	while (std::getline(configFile, line))
        configLines.push_back(line);
    configFile.close();

    // std::vector<Server> _servers;

    // check for server blocks and location blocks
    for (size_t i = 0; i < configLines.size(); i++) {
        if (configLines[i].find("server {") != std::string::npos) {
            std::vector<std::string> serverLines;
            std::vector<Location> _locations;
            while (++i < configLines.size()) {
                serverLines.push_back(configLines[i]);
                if (configLines[i].find("location") != std::string::npos) {
                    std::vector<std::string> locationLines;
                    while (i < configLines.size()) {
                        locationLines.push_back(configLines[i]);
                        // std::cout << configLines[i] << std::endl;
                        if (configLines[i].find("}") != std::string::npos) {
                            Location _location;

                            _location.uri = Path(getKeywordValues("location", locationLines)[0], URI);
                            
                            // detect HTTP redirection
                            std::vector<std::string> httpRedirection = getKeywordValues("return", locationLines);
                            if (httpRedirection.empty() == false) {
                                if (httpRedirection.size() != 2) {
                                    wsutils::errorExit("Http Redirection format should be: return <status code> <url>");
                                }
                                _location.isHttpRedirection = true;
                                _location.redirectionStatusCode = httpRedirection[0];
                                _location.redirectURL = httpRedirection[1];
                                _locations.push_back(_location);
                                i++;
                                break;  // ignore everything else in the block when it's a http redirection
                            }

                            // If location doesn't have root, get from server block
                            std::vector<std::string> roots = getKeywordValues("root", locationLines);
                            if (roots.empty()) {
                                _location.root = Path(getKeywordValues("root", serverLines)[0]);
                                _location.isCustomRoot = false;
                            } else {
                                _location.root = Path(getKeywordValues("root", locationLines)[0]);
                                _location.isCustomRoot = true;
                            }
                            wsutils::log("ROOT is: " + _location.root.getPath(), "./logs");
                            
                            // If location doesn't have index vector, get from server block
                            std::vector<std::string> index = getKeywordValues("index", locationLines);
                            _location.index = getKeywordValues("index", !index.empty() ? locationLines : serverLines);

                            // Autoindex
                            std::vector<std::string> autoindex = getKeywordValues("autoindex", locationLines);
                            _location.autoindex = autoindex.empty() ? false : getKeywordValues("autoindex", locationLines)[0] == "on" ? true : false;

                            // client_max_body_size
                            std::vector<std::string> mcbsStrs = getKeywordValues("client_max_body_size", locationLines);
                            _location.max_client_body_size = mcbsStrs.empty() ? 0 : wsutils::convertSizeStringToBytes(mcbsStrs[0]);

                            // upload_store
                            std::vector<std::string> uploadStoreStrs = getKeywordValues("upload_store", locationLines);
                            if (uploadStoreStrs.empty() == false) {
                                _location.upload_store = Path(uploadStoreStrs[0], IGNORE);
                                _location.accept_upload = true;
                            } else {
                                _location.accept_upload = false;
                            }

                            // limit_except (allowed HTTP methods)
                            std::vector<std::string> allowedHttpMethods = getKeywordValues("limit_except", locationLines);
                            if (allowedHttpMethods.empty() == false) {
                                _location.allowedHttpMethods = allowedHttpMethods;
                            }

                            _locations.push_back(_location);
                            i++;
                            break;
                        }
                        i++;
                    }
                }
                if (configLines[i].find("}") != std::string::npos) {
                    Server _server;
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
                        _server.hosts.push_back(std::make_pair(host, port));
                    }
                    _server.root = Path(getKeywordValues("root", serverLines)[0], DIRECTORY);
                    _server.index = getKeywordValues("index", serverLines);
                    _server.server_name = getKeywordValues("server_name", serverLines);
                    _server.cgi_extensions = getKeywordValues("cgi_extension", serverLines);

                    // [404, 404.html, 501, 501.html]
                    std::vector<std::string> errorPages = getKeywordValues("error_page", serverLines);
                    if (errorPages.size() % 2 != 0) {
                        wsutils::errorExit("Error Pages should be key-value pairs!");
                    }
                    // [404, 404.html] => 404 => root/404.html
                    for (std::vector<std::string>::iterator it = errorPages.begin(); it != errorPages.end(); it += 2) {
                        Path errorPagePath = _server.root.concat(*(it + 1), URI);
                        _server.error_pages[*it] = errorPagePath;
                    }
                    _server.locations = _locations;
                    _servers.push_back(_server);
                    i++;
                    break ;
                }
            }
        }
    }
}

std::vector<std::string> Parser::getKeywordValues(std::string keyword, std::vector<std::string> serverLines) {
    std::vector<std::string> values;
    for (size_t i = 0; i < serverLines.size(); ++i) {
        std::string value;
        if (serverLines[i].find(keyword) != std::string::npos) {
            std::istringstream iss(serverLines[i]);
            iss >> value;
            if (value == keyword)
                while (!iss.eof()) {
                    iss >> value;
                    if (iss.eof() && (value[value.length() - 1] == ';' || value[value.length() - 1] == '{'))
                        values.push_back(value.substr(0, value.size() - 1));
                    else
                        values.push_back(value);
                }
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

void Parser::print_values(std::vector<Server> _servers) {
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

Server::Server() {}

Location::Location() {
    isCustomRoot = false;
    isHttpRedirection = false;
    autoindex = false;
    accept_upload = false;
    allowedHttpMethods.push_back("GET");
    allowedHttpMethods.push_back("POST");
    allowedHttpMethods.push_back("DELETE");
}
