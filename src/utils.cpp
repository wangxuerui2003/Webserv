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