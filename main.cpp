#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

// std::vector<std::string> configLines;
// std::vector<std::string> serverBlock;
// std::map<std::string, std::vector<std::string>> serverValues;
// std::vector<std::string> locationBlock;
// std::map<std::string, std::vector<std::string>> locationValues;

struct Location {
	Location();
	
	std::vector<std::string> path;
	std::vector<std::string> root;
	// std::vector<std::string> allowedHttpMethods;
	// bool directory_listing;
	// std::vector<std::string> index;
};

struct Server {
	Server();

	std::vector<std::string> listen;
	std::vector<std::string> root;
	std::vector<std::string> index;
	std::vector<std::string> server_name;
	std::vector<std::string> error_page;
	// size_t max_client_body_size;

	std::vector<Location> locations;
};


Server::Server() {}

Location::Location() {}

std::vector<std::string> getKeywordValues(std::string keyword, std::vector<std::string> serverLines) {
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

int main(void)
{
    // Open config file.
    std::ifstream configFile("./conf/example.conf");
	if (!configFile.is_open()) {
        std::cerr << "Error opening config file." << std::endl;
        return 1;
    }

    // Store whole config file line by line into a vector.
    std::vector<std::string> configLines;
	std::string line;
	while (std::getline(configFile, line))
        configLines.push_back(line);
    configFile.close();

    std::vector<Server> servers;

    // check for server blocks
    for (size_t i = 0; i < configLines.size(); i++) {
        // START OF SERVER BLOCK
        if (configLines[i].find("server {") != std::string::npos) {
            // std::cout << i << " start of server: " << configLines[i] << std::endl;
            std::vector<std::string> serverLines;
            std::vector<Location> _locations;
            // START OF LOCATION BLOCK
            while (++i < configLines.size()) {
                serverLines.push_back(configLines[i]);
                if (configLines[i].find("location") != std::string::npos) {
                    // std::cout << i << " start of location: " << configLines[i] << std::endl;
                    std::vector<std::string> locationLines;
                    // END OF LOCATION BLOCK
                    while (i < configLines.size()) {
                        locationLines.push_back(configLines[i]);
                        std::cout << configLines[i] << std::endl;
                        if (configLines[i].find("}") != std::string::npos) {
                            // std::cout << i << " end of location: " << configLines[i] << std::endl;
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
                // END OF SERVER BLOCK
                if (configLines[i].find("}") != std::string::npos) {
                    // std::cout << i << " end of server: " << configLines[i] << std::endl;
                    Server _server;
                    _server.listen = getKeywordValues("listen", serverLines);
                    _server.root = getKeywordValues("root", serverLines);
                    _server.index = getKeywordValues("index", serverLines);
                    _server.server_name = getKeywordValues("server_name", serverLines);
                    _server.error_page = getKeywordValues("error_page", serverLines);
                    _server.locations = _locations;
                    servers.push_back(_server);
                    i++;
                    break ;
                }
            }
        }
    }

    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
        int i;
        i++;
        std::cout << "Server " << i << " - ";
        std::cout << "| listen: ";
        for (std::vector<std::string>::iterator it2 = it->listen.begin(); it2 != it->listen.end(); ++it2)
            std::cout << *it2 << " ";
        std::cout << "| root: ";
        for (std::vector<std::string>::iterator it2 = it->root.begin(); it2 != it->root.end(); ++it2)
            std::cout << *it2 << " ";
        std::cout << "| index: ";
        for (std::vector<std::string>::iterator it2 = it->index.begin(); it2 != it->index.end(); ++it2)
            std::cout << *it2 << " ";
        std::cout << "| server_name: ";
        for (std::vector<std::string>::iterator it2 = it->server_name.begin(); it2 != it->server_name.end(); ++it2)
            std::cout << *it2 << " ";
        std::cout << "| error_page: ";
        for (std::vector<std::string>::iterator it2 = it->error_page.begin(); it2 != it->error_page.end(); ++it2)
            std::cout << *it2 << " ";
        std::cout << "| locations[0].path: ";
        for (std::vector<std::string>::iterator it2 = it->locations[0].path.begin(); it2 != it->locations[0].path.end(); ++it2)
            std::cout << *it2 << " ";
        std::cout << "| locations[0].root: ";
        for (std::vector<std::string>::iterator it2 = it->locations[0].root.begin(); it2 != it->locations[0].root.end(); ++it2)
            std::cout << *it2 << " ";
        std::cout << "| locations[1].path: ";
        for (std::vector<std::string>::iterator it2 = it->locations[1].path.begin(); it2 != it->locations[1].path.end(); ++it2)
            std::cout << *it2 << " ";
        std::cout << "| locations[1].root: ";
        for (std::vector<std::string>::iterator it2 = it->locations[1].root.begin(); it2 != it->locations[1].root.end(); ++it2)
            std::cout << *it2 << " ";
        std::cout << std::endl;
    }

    return 0;
}