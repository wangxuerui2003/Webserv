/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 15:09:23 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/06 16:30:20 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"

SessionInfo::SessionInfo() {
	
}

Session::Session() {

}

Session::Session(std::string sessionStoreFile) {
	if (Path::isAccessible(sessionStoreFile.c_str())) {
		_sessionStore = Path(sessionStoreFile, REG_FILE);
		return;
	}

	// try open with write
	std::ofstream tryOpen(sessionStoreFile.c_str(), std::ios::out | std::ios::trunc);
	if (!tryOpen.is_open()) {
		throw Path::InvalidPathException(sessionStoreFile + " is not creatable");
	}
	tryOpen.close();
	_sessionStore = Path(sessionStoreFile, REG_FILE);
	_sessionStore << "sessionId|sessionData|expirationTimestamp\n";
}

Session& Session::operator=(const Session& other) {
	_sessionStore = other._sessionStore;
	return *this;
}


Session::~Session() {

}

const Path& Session::getSessionStorePath(void) const {
	return _sessionStore;
}

std::string Session::getSessionDataById(std::string sessionId) {
	(void)sessionId;
	return "\"sessiondata1=wxuerui;sessiondata2=2003\"";
}

// generate a length 16 random string (ascii 33 <= char <= 126)
std::string Session::generateNewSessionId(void) {
    char result[17];
    int i;
    for (i = 0; i < 16; ++i) {
        char c = rand() % 127;
        if (c < 33) {
            c += 33;
        }

        result[i] = c;
    }

    result[i] = '\0';
    return result;
}

