/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poll.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 11:41:11 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/03 14:35:24 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLL_HPP
#define POLL_HPP

#include "AConnectionHandler.hpp"
#include <sys/poll.h>

#define MAX_POLL_FDS 1024

class Poll : public AConnectionHandler {
	public:
		Poll(const std::vector<Server>& servers);
		~Poll();

		void serverListen(void);

	private:
		void initFds(void);

		struct pollfd _monitorFds[MAX_POLL_FDS];
		int _maxFd;
		
};

#endif