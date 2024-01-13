/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 16:02:56 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/13 16:11:14 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

// namespace wsutils {

	void wsutils::errorExit(const char *msg) {
		std::cerr << RED << msg << RESET << std::endl;
		exit(1);
	}
	
// }