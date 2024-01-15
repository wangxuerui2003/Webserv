#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

int main(void)
{
    std::ifstream configFile("./example.conf");
	if (!configFile.is_open())
	{
        std::cerr << "Error opening config file." << std::endl;
        return 1;
    }
    std::vector <std::string> configLines; // Stores each line of the config file in the vector
	std::string line;
	while (std::getline(configFile, line))
        configLines.push_back(line);
    configFile.close();

    std::map<std::string, std::string> configValues;

    for (size_t i = 0; i < configLines.size(); ++i)
    {
        std::string keyword, value;
        if (configLines[i].find("listen") != std::string::npos)
        {
            std::istringstream iss(configLines[i]);
            iss >> keyword >> value;
            configValues[keyword] = value.substr(0, value.size() - 1);
        }
        if (configLines[i].find("root") != std::string::npos)
        {
            std::istringstream iss(configLines[i]);
            iss >> keyword >> value;
            configValues[keyword] = value.substr(0, value.size() - 1);
        }
    }
     std::cout << configValues["listen"] << std::endl;
     std::cout << configValues["root"] << std::endl;

    // // Process the configuration lines
    // std::map<std::string, std::string> configValues;

    // for (size_t i = 0; i < configLines.size(); ++i) {
    //     if (configLines[i].find("listen") != std::string::npos) {
    //         // Example: listen 127.0.0.1:8080;
    //         std::istringstream iss(configLines[i]);
    //         std::string keyword, address;
    //         iss >> keyword >> address;
    //         configValues["listen"] = address;
    //     } else if (configLines[i].find("root") != std::string::npos) {
    //         // Example: root /var/www/html/;
    //         std::istringstream iss(configLines[i]);
    //         std::string keyword, rootPath;
    //         iss >> keyword >> rootPath;
    //         configValues["root"] = rootPath;
    //     }
    //     // Add more conditions for other configuration parameters...
    // }

    // // Access the parsed values
    // std::cout << "Listen Address: " << configValues["listen"] << std::endl;
    // std::cout << "Root Path: " << configValues["root"] << std::endl;

    // return 0;
}