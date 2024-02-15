/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:16:57 by zwong             #+#    #+#             */
/*   Updated: 2024/02/15 15:02:03 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "webserv.hpp"
#include "Path.hpp"

class Request {
public:
    Request(const std::string& rawReqString);

    const std::string& getMethod() const;
    const Path& getURI();
    const std::string& getVersion() const;
    const std::string& getHost() const;
    const std::string& getPort() const;
    std::string getHeader(const std::string& headerName) const;
    std::map<std::string, std::string> &getHeaderMap();
    const std::string& getBody() const;
    const std::string& getQueryParams(void) const;
    const std::map<std::string, std::string>& getCookies(void) const;
    std::string getCookieByName(std::string cookieName) const;

    void setBody(std::string body);
    void setURI(Path& uri);

    class InvalidRequestPathException : public std::exception {
        const char *what() const throw();
    };

    std::string formatRequestPath(std::string reqPath);

private:
    std::string _method;
    Path _uri;
    std::string _httpVersion;
    std::string _host;
    std::string _port;
    std::map<std::string, std::string> _headers;
    std::string _body;
    std::string _queryParams;
    std::map<std::string, std::string> _cookies;

    void parseRequest(const std::string& rawReqString);
    void parseHeaders(const std::string& headerPart);
    void parseCookies(const std::string& cookies);
};

#endif