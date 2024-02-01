/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 16:01:20 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/01 22:30:02 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define COMMON_BUFFER_SIZE 1024

#include "Parser.hpp"
#include "Path.hpp"
#include "ConnectionHandler.hpp"

#include "colors.hpp"
#include "utils.hpp"

#include <cstdlib>

#define USAGE "Usage: ./webserv <config_file>"
#define DEFAULT_CONFIG "./conf/local.conf"
#define HTTP_REQUEST_TERMINATOR "\r\n\r\n"


#ifdef __linux__
	#include <errno.h>
#endif

#endif