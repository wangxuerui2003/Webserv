/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:46:49 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/06 16:27:46 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_HPP
#define SESSION_HPP

#include "webserv.hpp"
#include "Path.hpp"

struct SessionInfo {
	SessionInfo();

	std::string sessionId;
	std::string sessionData;
	time_t expirationTimestamp;
};

class Session {
	public:
		Session();
		Session(std::string sessionStoreFile);
		Session& operator=(const Session& other);
		~Session();

		std::string getSessionDataById(std::string sessionId);
		std::string addNewSession(std::string sessionData, unsigned int expireAfterSeconds);
		const Path& getSessionStorePath(void) const;
		static std::string generateNewSessionId(void);

	private:

		Path _sessionStore;
		

};


#endif