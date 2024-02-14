/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wxuerui@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 14:51:16 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/14 19:23:58 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATH_HPP
#define PATH_HPP

#include "webserv.hpp"

enum pathType {
	REG_FILE,
	DIRECTORY,
	SYM_LINK,
	URI,
	IGNORE,
	ERROR_PATH
};

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

		static Path mapURLToFS(const Path& reqestUri, const Path& uriPrefix, const Path& root, bool isCustomRoot);

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
		void write(std::string content);

		std::string getFileExtension(void) const;

		std::string urlDecode(std::string &str);

		Path getDirectory(void) const;
		std::string getFilename(void) const;

		bool operator==(const Path& other);
		bool operator!=(const Path& other);

		std::vector<std::string> readLines(void) const;
		void writeLines(std::vector<std::string>& lines) const;

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

		Path& operator<<(std::string text);

	private:
		std::string _path;
		enum pathType _type;
};

#endif