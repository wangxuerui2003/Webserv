/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 14:51:16 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/01 12:05:56 by zwong            ###   ########.fr       */
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

		Path concat(Path& other);
		Path concat(std::string otherPath, enum pathType type);
		Path concat(std::string otherPath);

		Path prepend(Path& other);
		Path prepend(std::string otherPath, enum pathType type);

		bool isReadable(void) const;
		bool isWritable(void) const;
		bool isExecutable(void) const;

		std::string read(void) const;
		static void write(std::string filePath, std::string content);

		std::string generateDirectoryListing(void) const;

		std::string getFileExtension(void) const;

		std::string urlDecode(std::string &str);

		Path getDirectory(void) const;
		std::string getFilename(void) const;

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