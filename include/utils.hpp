/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 16:07:17 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/23 21:07:03 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <fstream>
#include "colors.hpp"

namespace wsutils {
	void errorExit(const char *msg);
	void warningOutput(const char *msg);
	size_t convertSizeStringToBytes(const std::string& sizeString);
	void log(std::string msg, std::ostream& is);
	void log(std::string msg, std::string filename);
}

#endif