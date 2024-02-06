/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 16:07:17 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/06 21:42:02 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "webserv.hpp"

namespace wsutils {
	void errorExit(const char *msg);
	void warningOutput(const char *msg);
	size_t convertSizeStringToBytes(const std::string& sizeString);
	void log(std::string msg, std::ostream& os);
	void log(std::string msg, std::string filename);
	bool isIPv4(const std::string& str);
	bool isCorrectIP(std::string reqIP, std::string hostIP);
	std::string unchunkRequest(std::string& req);

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
			return 0;
		}

		return value;
	}
}

#endif