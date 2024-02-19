/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 16:01:20 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/19 09:19:39 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define READ_BUFFER_SIZE 1024

#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>

#include <algorithm>
#include <list>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>
#include <exception>
#include <vector>
#include <map>

#include "colors.hpp"
#include "utils.hpp"


#define USAGE "Usage: ./webserv <config_file>"
#define DEFAULT_CONFIG "./conf/local.conf"
#define HTTP_HEADER_TERMINATOR "\r\n\r\n"

#define WEBSERV_SESSION_ID_NAME "webserv_session_id"
#define SESSION_DATA_CGI_HEADER "WEBSERV_SESSION_DATA"


#ifdef __linux__
	#include <errno.h>
#endif

#endif