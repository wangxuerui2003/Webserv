/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:46:49 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/04 15:23:35 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_HPP
#define SESSION_HPP

#include "webserv.hpp"
#include "Path.hpp"

struct SessionData {
	SessionData();

	std::string sessionId;
	std::map<std::string, std::string> sessionData;
	time_t expirationDate;
};

class Session {
	public:
		Session();
		~Session();

		SessionData *getSessionDataById(std::string sessionId);
		void addNewSession(SessionData& sessionData);
		static std::string generateNewSessionId(void);

	private:
		Path sessionStore;
		

};


#endif