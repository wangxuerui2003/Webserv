/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 14:51:16 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/13 17:48:14 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATH_HPP
#define PATH_HPP

#include <string>
#include <exception>

enum pathType {
	REG_FILE,
	DIRECTORY,
	URL,
	ERROR_PATH
};

class Path {
	public:
		Path();
		Path(std::string path, enum pathType type);
		Path(const Path& copy);
		Path& operator=(const Path& copy);
		~Path();

		const std::string& getPath(void) const;
		const enum pathType& getType(void) const;

		int isParentDirOf(std::string otherPath);

		class InvalidPathException : public std::exception {
			const char *what() const throw();
		};

	private:
		std::string _path;
		enum pathType _type;
};

#endif