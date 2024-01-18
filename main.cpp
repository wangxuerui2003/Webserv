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
	std::vector<std::string> error_pages;
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
                    if (iss.eof() && value.back() == ';')
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
    std::vector<Server> servers;

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

    Server _server;
    // check for server blocks
    for (size_t i = 0; i < configLines.size(); i++) {
        // START OF SERVER BLOCK
        if (configLines[i].find("server {") != std::string::npos) {
            std::cout << i << " start of server: " << configLines[i] << std::endl;
            // START OF LOCATION BLOCK
            while (++i < configLines.size()) {
                if (configLines[i].find("location") != std::string::npos) {
                    std::cout << i << " start of location: " << configLines[i] << std::endl;
                    // END OF LOCATION BLOCK
                    while (++i < configLines.size()) {
                        if (configLines[i].find("}") != std::string::npos) {
                            std::cout << i << " end of location: " << configLines[i] << std::endl;
                            i++;
                            break;
                        }
                    }
                }
                // END OF SERVER BLOCK
                if (configLines[i].find("}") != std::string::npos) {
                    std::cout << i << " end of server: " << configLines[i] << std::endl;
                    servers.push_back(_server);
                    i++;
                    break ;
                }
            }
        }
    }

    // Add values to struct and add to servers vector.
    // Server _server;
    // _server.listen = getKeywordValues("listen", configLines);
    // _server.root = getKeywordValues("root", configLines);
    // _server.index = getKeywordValues("index", configLines);
    // _server.server_name = getKeywordValues("server_name", configLines);

    // servers.push_back(_server);

    // Iterator
    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
        std::cout << "server_name: ";
        for (std::vector<std::string>::iterator it2 = it->server_name.begin(); it2 != it->server_name.end(); ++it2) {
            std::cout << *it2 << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}