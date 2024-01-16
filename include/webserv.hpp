/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 16:01:20 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/15 20:53:55 by wxuerui          ###   ########.fr       */
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

#endif