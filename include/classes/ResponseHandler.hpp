/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHandler.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 18:27:18 by zwong             #+#    #+#             */
/*   Updated: 2024/01/16 16:06:51 by zwong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include "Request.hpp"

class ResponseHandler {
    public:
        static std::string generateResponse(const std::string& rawReqString);

    private:
    static bool isCgiRequest(const std::string& path);
    static std::string getContentType(const std::string& filePath);
    static std::string handleStaticContent(const Request& httpRequest);
    static std::string readFile(const std::string& filename);
};

#endif