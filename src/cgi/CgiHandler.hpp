/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 18:09:30 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/06 12:42:54 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "webserv.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Path.hpp"

class CgiHandler {
public:
    // Method to handle CGI execution
    static std::string handleCgi(Request &request, Server &server, Location &location);
    static std::string	parseCgiOutput(int pipefd_output, int pipefd_error);

private:
    // Private helper methods for CGI handling
    static char **setEnv(Request &request, Server& server, Location& location, Path& cgiPath);
};

#endif
