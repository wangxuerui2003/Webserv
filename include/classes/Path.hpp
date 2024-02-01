/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 14:51:16 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/01 18:17:31 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATH_HPP
#define PATH_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <exception>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <dirent.h>

#include "utils.hpp"

enum pathType {
	REG_FILE,
	DIRECTORY,
	SYM_LINK,
	URI,
	IGNORE,
	ERROR_PATH
};

struct Location;

class Path {
	public:
		Path();
		Path(std::string path);  // Construct with auto detect path type
		Path(std::string path, enum pathType expectedType);  // Construct with expected Type
		Path(const Path& copy);
		Path& operator=(const Path& copy);
		~Path();

		const std::string& getPath(void) const;
		const enum pathType& getType(void) const;

		static bool isAccessible(const char *path);

		static Path mapURLToFS(Path& reqestUri, Path& uriPrefix, Path& root, bool isCustomRoot);

		static Location *getBestFitLocation(std::vector<Location>& locations, Path& requestUri);

		static enum pathType getFileType(const char *path);

		Path concat(Path& other) const;
		Path concat(std::string otherPath, enum pathType type) const;
		Path concat(std::string otherPath) const;

		Path prepend(Path& other) const;
		Path prepend(std::string otherPath, enum pathType type) const;

		bool isReadable(void) const;
		bool isWritable(void) const;
		bool isExecutable(void) const;

		std::string read(void) const;
		static void write(std::string filePath, std::string content);

		std::string generateDirectoryListing(Path& uri) const;

		std::string getFileExtension(void) const;

		std::string urlDecode(std::string &str);

		Path getDirectory(void) const;
		std::string getFilename(void) const;

		bool operator==(const Path& other);
		bool operator!=(const Path& other);

		class InvalidPathException : public std::exception {
			private:
				std::string _errorMsg;
			public:
				InvalidPathException(std::string errorMsg);
				~InvalidPathException() throw();
				const char *what() const throw();
		};

		class InvalidOperationException : public std::exception {
			private:
				std::string _errorMsg;
			public:
				InvalidOperationException(std::string errorMsg);
				~InvalidOperationException() throw();
				const char *what() const throw();
		};

	private:
		std::string _path;
		enum pathType _type;
};

#include "Parser.hpp"

#endif