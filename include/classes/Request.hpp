/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 17:16:57 by zwong             #+#    #+#             */
/*   Updated: 2024/01/16 11:15:54 by zwong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <map>
#include <algorithm>

class Request {
public:
    Request(const std::string& rawReqString);

    std::string getMethod() const;
    std::string getPath() const;
    std::string getVersion() const;
    std::string getHeader(const std::string& headerName) const;
    std::string getBody() const;

private:
    std::string _method;
    std::string _path;
    std::string _httpVersion;
    std::map<std::string, std::string> _headers;
    std::string _body;

    void parseRequest(const std::string& rawReqString);
    void parseHeaders(const std::string& headerPart);
};

#endif