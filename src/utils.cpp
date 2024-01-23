/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 16:02:56 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/15 20:41:58 by wxuerui          ###   ########.fr       */
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