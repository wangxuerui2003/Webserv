/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 17:48:35 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/22 10:40:04 by zwong            ###   ########.fr       */
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

    std::ifstream configFile(configFilePath);
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
                        std::cout << configLines[i] << std::endl;
                        if (configLines[i].find("}") != std::string::npos) {
                            Location _location;
                            _location.path = getKeywordValues("location", locationLines);
                            _location.root = getKeywordValues("root", locationLines);
                            _locations.push_back(_location);
                            i++;
                            break;
                        }
                        i++;
                    }
                }
                if (configLines[i].find("}") != std::string::npos) {
                    Server _server;
                    _server.listen = getKeywordValues("listen", serverLines);
                    _server.root = getKeywordValues("root", serverLines);
                    _server.index = getKeywordValues("index", serverLines);
                    _server.server_name = getKeywordValues("server_name", serverLines);
                    _server.error_page = getKeywordValues("error_page", serverLines);
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
                    if (iss.eof() && (value.back() == ';' || value.back() == '{'))
                        values.push_back(value.substr(0, value.size() - 1));
                    else
                        values.push_back(value);
                }
        }
    }
    return values;
}

Server::Server() {}

Location::Location() {}