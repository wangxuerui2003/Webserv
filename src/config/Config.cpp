/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 13:03:19 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/04 15:22:37 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Server::Server() {}

Location::Location() {
    isCustomRoot = false;
    isHttpRedirection = false;
    autoindex = false;
    accept_upload = false;
    session = NULL;
    allowedHttpMethods.push_back("GET");
    allowedHttpMethods.push_back("POST");
    allowedHttpMethods.push_back("DELETE");
}