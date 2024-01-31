/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 17:48:35 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/31 08:51:44 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

// Initialize the instance pointer
Parser *Parser::_instance = NULL;

Parser::Parser() {
}

Parser *Parser::getInstance() {
	if (_instance == NULL) {
		_instance = new Parser();
	}

	return _instance;
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
                                _location.upload_store = uploadStoreStrs[0];
                                _location.accept_upload = true;
                            } else {
                                _location.accept_upload = false;
                            }

                            // limit_except (allowed HTTP methods)
                            _location.allowedHttpMethods = getKeywordValues("limit_except", locationLines);
                            if (_location.allowedHttpMethods.empty()) {
                                _location.allowedHttpMethods.push_back("GET");
                                _location.allowedHttpMethods.push_back("POST");
                                _location.allowedHttpMethods.push_back("DELETE");
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

	// Server dummy;
	// dummy.root = Path("./www", DIRECTORY);
	// dummy.host = "127.0.0.1";
	// dummy.port = "8080";
	// dummy.index.push_back("index.html");
	// dummy.error_pages[404] = Path("./www/404.html", REG_FILE);

	// Location location;
	// location.url = "/";
	// location.root = dummy.root;
	// location.index = dummy.index;

	// dummy.locations.push_back(location);

	// _servers.push_back(dummy);
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

Server::Server() {}

Location::Location() {
    isCustomRoot = false;
}