/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:46:49 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/06 12:40:00 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_HPP
#define SESSION_HPP

#include "webserv.hpp"
#include "Path.hpp"

struct SessionInfo {
	SessionInfo();

	std::string sessionId;
	std::map<std::string, std::string> sessionData;
	time_t expirationDate;
};

class Session {
	public:
		Session(std::string sessionStoreFile);
		~Session();

		std::string getSessionDataById(std::string sessionId);
		void addNewSession(SessionInfo& sessionData);
		static std::string generateNewSessionId(void);
		const Path& getSessionStorePath(void) const;

	private:
		Path _sessionStore;
		

};


#endif