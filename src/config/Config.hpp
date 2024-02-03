/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 13:02:24 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/03 16:25:23 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "webserv.hpp"
#include "Path.hpp"

enum event {
	SELECT,
	POLL
};

struct Location {
	Location();

	Path uri;
	Path root;
	bool isCustomRoot;
	std::vector<std::string> index;
	bool autoindex;
	size_t max_client_body_size;
	Path upload_store;
	bool accept_upload;
	std::vector<std::string> allowedHttpMethods;

	bool isHttpRedirection;
	std::string redirectionStatusCode;
	std::string redirectURL;
};

struct Server {
	Server();

	std::vector<std::pair<std::string, std::string> > hosts;
	std::vector<std::string> server_name;
	Path root;
	std::vector<std::string> index;
	std::map<std::string, Path> error_pages;
	std::map<std::string, std::string> cgiHandlers;

	std::vector<Location> locations;
};

#endif