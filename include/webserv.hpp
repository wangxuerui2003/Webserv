/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 16:01:20 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/23 20:44:52 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Parser.hpp"
#include "Path.hpp"
#include "ConnectionHandler.hpp"

#include "colors.hpp"
#include "utils.hpp"

#include <cstdlib>

#define USAGE "Usage: ./webserv <config_file>"
#define DEFAULT_CONFIG "./conf/example.conf"
#define HTTP_REQUEST_TERMINATOR "\r\n\r\n"

#ifdef __linux__
	#include <errno.h>
#endif

#endif