/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 16:07:17 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/01 22:21:18 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include "colors.hpp"

namespace wsutils {
	void errorExit(const char *msg);
	void warningOutput(const char *msg);
	size_t convertSizeStringToBytes(const std::string& sizeString);
	void log(std::string msg, std::ostream& os);
	void log(std::string msg, std::string filename);

	template <typename T>
	std::string toString(const T& arg) {
		std::ostringstream ss;

		ss << arg;

		return ss.str();
	}


	template <typename T>
	T stringToNumber(std::string str) {
		T value;
		std::istringstream ss(str);

		if (!(ss >> value)) {
			throw std::runtime_error("Conversion from string to int failed.");
		}

		return value;
	}
}

#endif