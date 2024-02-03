/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 18:09:30 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/03 11:57:45 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "webserv.hpp"
#include "Request.hpp"
#include "Response.hpp"

class CgiHandler {
public:
    // Constructor
    CgiHandler();

    // Destructor
    ~CgiHandler();

    // Method to handle CGI execution
    std::string handleCgi(Request &request, Server &server, Location &location);
    std::string	parseCgiOutput(int pipefd_output, int pipefd_error);

private:
    // Private helper methods for CGI handling
    char **setEnv(Request &request, Location& location, Path& cgiPath);
};

#endif
