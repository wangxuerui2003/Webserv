/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 15:09:23 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/06 21:42:24 by wxuerui          ###   ########.fr       */
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
	removeExpiredSessions();
	// TODO: read the file into a linked list, ignore the first line, check one by one see if there is a match
	(void)sessionId;
	return "sessiondata1=wxuerui;sessiondata2=2003";
}

void Session::removeExpiredSessions(void) {
	std::vector<std::string> *lines = _sessionStore.readLines<std::vector<std::string> >();
	std::vector<std::string>::iterator it = lines->begin();
	std::string remainingSessions = *it + '\n';

	++it;  // ignore the csv header line

	std::time_t now = std::time(NULL);
	for (; it != lines->end(); ++it) {
		size_t lastSeparator = it->find_last_of('|');
		std::string timestamp = it->substr(lastSeparator + 1);
		std::time_t expirationTimestamp = wsutils::stringToNumber<time_t>(timestamp);
		if (expirationTimestamp < now) {
			continue;
		}
		remainingSessions += *it + '\n';
	}

	delete lines;
	_sessionStore.write(remainingSessions);
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

