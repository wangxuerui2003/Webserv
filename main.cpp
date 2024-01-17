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
	
	std::string url;
	Path root;
	// std::vector<std::string> allowedHttpMethods;
	// bool directory_listing;
	std::vector<std::string> index;
};

struct Server {
	Server();

	std::vector<std::string> port;
	std::vector<std::string> host;
	std::vector<std::string> server_name;
	Path root;
	std::vector<std::string> index;
	std::map<int, Path> error_pages;
	// size_t max_client_body_size;

	std::vector<Location> locations;
};

void getKeywordValues(std::string keyword, std::map<std::string, std::vector<std::string> > &serverValues, std::vector<std::string> lines) {
    for (size_t i = 0; i < lines.size(); ++i) {
        std::string value;
        if (lines[i].find(keyword) != std::string::npos) {
            std::vector<std::string> values;
            std::istringstream iss(lines[i]);
            iss >> value;
            while (!iss.eof()) {
                iss >> value;
                if (iss.eof() && value.back() == ';')
                    values.push_back(value.substr(0, value.size() - 1));
                else
                    values.push_back(value);
            }
            serverValues[keyword] = values;
        }
    }
}

int main(void)
{
    // open config file
    std::ifstream configFile("./conf/example.conf");
	if (!configFile.is_open()) {
        std::cerr << "Error opening config file." << std::endl;
        return 1;
    }

    // stores lines into a vector
    std::vector<std::string> configLines;
	std::string line;
	while (std::getline(configFile, line))
        configLines.push_back(line);
    configFile.close();

    // check for server blocks

    // add keyword and values to a map
    std::map<std::string, std::vector<std::string> > serverValues;
    getKeywordValues("listen", serverValues, configLines);
    getKeywordValues("root", serverValues, configLines);
    getKeywordValues("index", serverValues, configLines);
    getKeywordValues("location", serverValues, configLines);

    // iterrate the parsed values
    std::map<std::string, std::vector<std::string> >::iterator iter;
    for (iter = serverValues.begin(); iter != serverValues.end(); ++iter) {
        std::cout << "Key: " << iter->first << " | Values: ";
        for (size_t i = 0; i < iter->second.size(); ++i) {
            std::cout << iter->second[i] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}