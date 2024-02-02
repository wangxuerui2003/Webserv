/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 16:02:56 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/02 12:47:38 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void wsutils::errorExit(const char *msg) {
	std::cerr << RED << msg << RESET << std::endl;
	exit(1);
}

void wsutils::warningOutput(const char *msg) {
	std::cerr << YELLOW << msg << RESET << std::endl;
}

size_t wsutils::convertSizeStringToBytes(const std::string& sizeString) {
    size_t multiplier = 1;
    size_t sizeValue = 0; 
	int size = std::atoi(sizeString.c_str());
	if (size <= 0) {
		sizeValue = 0;
	} else {
		sizeValue = size;
	}

    // Check for the unit (K, M, G) and apply the multiplier
    if (sizeString.find_first_of("Kk") != std::string::npos) {
        multiplier = 1024;
    } else if (sizeString.find_first_of("Mm") != std::string::npos) {
        multiplier = 1024 * 1024;
    } else if (sizeString.find_first_of("Gg") != std::string::npos) {
        multiplier = 1024 * 1024 * 1024;
    }

    return sizeValue * multiplier;
}

void wsutils::log(std::string msg, std::ostream& os) {
    os << "----------LOG BEGIN----------\n";
    os << YELLOW << msg << RESET;
    os << "\n----------LOG END----------" << std::endl;
}

void wsutils::log(std::string msg, std::string filename) {
    std::ofstream outputFile(filename.c_str(), std::ios::app);

    if (!outputFile.is_open()) {
        throw std::ios_base::failure("Fail to open file");
    }
    
    // outputFile << "----------LOG BEGIN----------\n";
    outputFile << msg << std::endl;
    // outputFile << "\n----------LOG END----------" << std::endl;

    outputFile.close();
}

bool wsutils::isIPv4(const std::string& str) {
    if (str == "localhost") {
        return true;
    }

    std::istringstream iss(str);
    std::string segment;
    int num;
    char delim = '.';
    int count = 0;

    while (std::getline(iss, segment, delim)) {
        if (!(std::istringstream(segment) >> num) || num < 0 || num > 255) {
            return false; // Invalid segment
        }
        count++;
    }

    return count == 4; // Check for exactly four segments
}

bool wsutils::isCorrectIP(std::string reqIP, std::string hostIP) {
    return (reqIP == hostIP || (reqIP == "localhost" && hostIP == "127.0.0.1") || hostIP == "0.0.0.0");
}
