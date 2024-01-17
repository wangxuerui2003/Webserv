#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

int main(void)
{
    // open config file
    std::ifstream configFile("./example.conf");
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
    std::string keyword = "listen";

    for (size_t i = 0; i < configLines.size(); ++i) {

        std::string value;
        if (configLines[i].find(keyword) != std::string::npos) {
            std::vector<std::string> values;
            std::istringstream iss(configLines[i]);
            iss >> value;
            while (!iss.eof()) {
                iss >> value;
                if (iss.eof())
                    values.push_back(value.substr(0, value.size() - 1));
                else
                    values.push_back(value);
            }
            serverValues[keyword] = values;
        }
    }

    // Access the parsed values
    std::vector<std::string> listenValues = serverValues[keyword];
    std::cout << "Values: ";
    for (size_t i = 0; i < listenValues.size(); ++i) {
        std::cout << listenValues[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}