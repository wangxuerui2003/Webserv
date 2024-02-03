/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:16:57 by zwong             #+#    #+#             */
/*   Updated: 2024/02/03 12:59:57 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "webserv.hpp"
#include "Path.hpp"

class Request {
public:
    Request(const std::string& rawReqString);

    std::string getMethod() const;
    Path&       getURI();
    std::string getVersion() const;
    std::string getHost() const;
    std::string getPort() const;
    std::string getHeader(const std::string& headerName) const;
    std::map<std::string, std::string> &getHeaderMap();
    std::string getBody() const;
    const std::string& getQueryParams(void) const;

    void setBody(std::string body);
    void setURI(Path& uri);

private:
    std::string _method;
    Path _uri;
    std::string _httpVersion;
    std::string _host;
    std::string _port;
    std::map<std::string, std::string> _headers;
    std::string _body;
    std::string _queryParams;

    void parseRequest(const std::string& rawReqString);
    void parseHeaders(const std::string& headerPart);
};

#endif