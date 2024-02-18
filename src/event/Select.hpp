/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Select.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 11:41:11 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/18 15:46:50 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SELECT_HPP
#define SELECT_HPP

#include "AConnectionHandler.hpp"
#include <sys/select.h>

class Select : public AConnectionHandler {
	public:
		Select(const std::vector<Server>& servers);
		~Select();

		void serverListen(void);

	private:
		void initFds(void);

		fd_set _readFds;
		fd_set _writeFds;
		fd_set _exceptFds;
		int _maxFd;
};

#endif