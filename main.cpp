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

    // check for server blocks
    std::vector<std::string> serverLines;
    // while (std::getline(configFile, line)) {
    //     if (line.find("server {") != std::string::npos) {
    //         while (line.find("}") == std::string::npos)
    //             serverLines.push_back(line);
    //     }
    // }
    bool in_block = false;
    for (size_t i = 0; i < configLines.size(); i++) {
        if (configLines[i].find("server {") != std::string::npos)
            in_block = true;
        else if (configLines[i].find("location")) {
            
        }
        else if (configLines[i].find("}") != std::string::npos)
            break ;
        if (in_block == true)
            serverLines.push_back(line);
    }
    for (size_t i = 0; i < serverLines.size(); i++)
        std::cout << serverLines[i] << std::endl;

    // Add values to struct and add to servers vector.
    Server _server;
    _server.listen = getKeywordValues("listen", configLines);
    _server.root = getKeywordValues("root", configLines);
    _server.index = getKeywordValues("index", configLines);

    servers.push_back(_server);

    // Iterator
    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
        std::cout << "index: ";
        for (std::vector<std::string>::iterator it2 = it->index.begin(); it2 != it->index.end(); ++it2) {
            std::cout << *it2 << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}