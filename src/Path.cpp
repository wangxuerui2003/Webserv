/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 17:48:28 by wxuerui           #+#    #+#             */
/*   Updated: 2024/01/13 18:09:21 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Path.hpp"

Path::Path() : _path(""), _type(ERROR_PATH) {

}

Path::Path(std::string path, enum pathType type) : _path(path), _type(type) {
	// TODO: throw exception if the path is invalid or type is not the same as input
}

Path::Path(const Path& copy) {
	*this = copy;
}

Path& Path::operator=(const Path& copy) {
	_path = copy.getPath();
	_type = copy.getType();
	return *this;
}

Path::~Path() {

}

int Path::isParentDirOf(std::string otherPath) {
	// TODO
	(void)otherPath;
	return 0;
}

const std::string& Path::getPath(void) const {
	return _path;
}

const enum pathType& Path::getType(void) const {
	return _type;
}

const char *Path::InvalidPathException::what() const throw() {
	return "Invalid Path";
}