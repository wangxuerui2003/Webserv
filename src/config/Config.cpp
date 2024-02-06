/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 13:03:19 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/06 16:30:01 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Server::Server() {
    hasSessionManagement = false;
}

Server::~Server() {
}

Location::Location() {
    isCustomRoot = false;
    isHttpRedirection = false;
    autoindex = false;
    accept_upload = false;
    allowedHttpMethods.push_back("GET");
    allowedHttpMethods.push_back("POST");
    allowedHttpMethods.push_back("DELETE");
}

Config::Config() {
    eventHandlerType = SELECT;
}