/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 15:09:23 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/06 12:45:52 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"

SessionInfo::SessionInfo() {
	
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
}


Session::~Session() {

}

const Path& Session::getSessionStorePath(void) const {
	return _sessionStore;
}

std::string Session::getSessionDataById(std::string sessionId) {
	return "\"sessiondata1=wxuerui;sessiondata2=2003\"";
}


