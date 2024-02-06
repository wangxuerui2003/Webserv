/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 15:09:23 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/06 18:58:59 by wxuerui          ###   ########.fr       */
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
	// TODO
	(void)sessionId;
	return "\"sessiondata1=wxuerui;sessiondata2=2003\"";
}

// generate a length 16 random string
// 0-9,   A-Z,   a-z
// 48-57, 65-98, 97-122
std::string Session::generateNewSessionId(void) {
    char result[17];
    int i = 0;
    while (i < 16) {
        char c = rand() % 127;
        if ((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122)) {
			result[i] = c;
			++i;
        }

    }

    result[i] = '\0';
    return result;
}

std::string Session::addNewSession(std::string sessionData, time_t expireAfterSeconds) {
	std::string sessionId = generateNewSessionId();
	time_t expirationTimestamp = std::time(NULL) + expireAfterSeconds;

	_sessionStore << sessionId + '|' + sessionData + '|' + wsutils::toString(expirationTimestamp) + '\n';
	return sessionId;
}

