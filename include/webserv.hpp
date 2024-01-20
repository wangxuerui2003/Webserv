/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 16:01:20 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/20 12:27:24 by wxuerui          ###   ########.fr       */
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
#define DEFAULT_CONFIG "./conf/test.conf"
#define HTTP_REQUEST_TERMINATOR "\r\n\r\n"

#ifdef __linux__
	#include <errno.h>
#endif

#endif