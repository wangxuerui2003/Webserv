/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 17:48:28 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/06 21:39:59 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Path.hpp"

Path::Path() : _path(""), _type(ERROR_PATH) {}

// decodes url that has %20 to " " (spaces), or %50 etc...
std::string Path::urlDecode(std::string &str) {
    std::stringstream decoded;
    char ch;
    size_t i = 0;
    while (i < str.length()) {
        if (str[i] == '%' && i + 2 < str.length()) {
            int hex;
            std::istringstream(str.substr(i + 1, 2)) >> std::hex >> hex;
            ch = static_cast<char>(hex);
            decoded << ch;
            i += 3;
        } else {
            decoded << str[i];
            ++i;
        }
    }
	// wsutils::log("decoded string is: " + decoded.str(), "./logs");
    return (decoded.str());
}

// Auto detect the path type (regfile, symlink or directory only), throw error if invalid path.
Path::Path(std::string path) : _path(urlDecode(path)) {
	// Remove trailing '/' to avoid error in stat() function
	if (_path != "/" && _path[_path.length() - 1] == '/') {
		_path.erase(_path.length() - 1);
	}

	enum pathType type = getFileType(_path.c_str());
	if (type == ERROR_PATH) {
		throw InvalidPathException(path + " is an invalid filesystem path");
	}

	_type = type;
}

// throw exception if the path is invalid or type is not the same as input
Path::Path(std::string path, enum pathType expectedType) : _path(urlDecode(path)), _type(expectedType) {
	if (_type == ERROR_PATH) {
		throw InvalidOperationException(path + " is an invalid filesystem path");
	}

	// Remove trailing '/' to avoid error in stat() function
	if (_path != "/" && _path[_path.length() - 1] == '/') {
		_path.erase(_path.length() - 1);
	}

	enum pathType realPathType = getFileType(_path.c_str());

	if (_type != URI && _type != IGNORE) {
		// Change path type to it's real type if expected type is wrong
		if (realPathType != _type) {
			throw InvalidOperationException("Expected file type of " + path + " does not match the real file type");
		}

		if (isAccessible(_path.c_str()) == false) {
			throw InvalidPathException(path + " is not accessible");
		}
	}

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

const std::string& Path::getPath(void) const {
	return _path;
}

const enum pathType& Path::getType(void) const {
	return _type;
}

/**
 * @brief
 * Use cases:
 * - REG_FILE/DIRECTORY/SYM_LINK cannot access the path
 * 
*/
Path::InvalidPathException::InvalidPathException(std::string errorMsg) : _errorMsg(errorMsg) {
	
}

Path::InvalidPathException::~InvalidPathException() throw() {
	
}

const char *Path::InvalidPathException::what() const throw() {
	return _errorMsg.c_str();
}

/**
 * @brief
 * Invalid operations:
 * - Construct with wrong path type
 * - Call concat but the _type is not a DIRECTORY
 * - Call prepend but other.getType() is not a DIRECTORY
*/
Path::InvalidOperationException::InvalidOperationException(std::string errorMsg) : _errorMsg(errorMsg) {
	
}

Path::InvalidOperationException::~InvalidOperationException() throw() {
	
}

const char *Path::InvalidOperationException::what() const throw() {
	return _errorMsg.c_str();
}

/**
 * @param path: the C style string path
 * 
 * @brief Check if the path is able to access (valid path)
 * 
 * @return bool
*/
bool Path::isAccessible(const char *path) {
	return (access(path, F_OK) == 0);
}

/**
 * @param path: the C style string path
 * 
 * @brief Get the type of file of a path
 * 
 * @return enum pathType
*/
enum pathType Path::getFileType(const char *path) {
	struct stat fileStat;

	if (stat(path, &fileStat) == 0) {
		if (S_ISREG(fileStat.st_mode)) {
			return REG_FILE;
		} else if (S_ISDIR(fileStat.st_mode)) {
			return DIRECTORY;
		} else if (S_ISLNK(fileStat.st_mode)) {
			return SYM_LINK;
		}
	}

	return ERROR_PATH;
}

bool Path::isReadable(void) const {
	return (access(_path.c_str(), R_OK) == 0);
}

bool Path::isWritable(void) const {
	return (access(_path.c_str(), W_OK) == 0);
}

bool Path::isExecutable(void) const {
	return (access(_path.c_str(), X_OK) == 0);
}

/**
 * @brief Concatenate another path to the current path object and return a new path object.
*/
Path Path::concat(Path& other) const {
	return concat(other.getPath(), other.getType());
}

/**
 * @brief Concatenate another path in the form of string and type to the current path object
 * and return a new path object.
*/
Path Path::concat(std::string otherPath, enum pathType type) const {
	if (type != IGNORE && _type != DIRECTORY) {
		throw InvalidOperationException(_path + " is not a directory");
	}
	
	std::string delimiter = "/";
	if (_path[_path.length() - 1] == '/') {
		delimiter = "";
	}

	if (otherPath[0] == '/') {
		return Path(_path + delimiter + otherPath.substr(1), type);
	}
	return Path(_path + delimiter + otherPath, type);
}

/**
 * @brief Concatenate another path in the form of string and type to the current path object
 * and return a new path object.
*/
Path Path::concat(std::string otherPath) const {
	if (_type != DIRECTORY) {
		throw InvalidOperationException(_path + " is not a directory");
	}
	
	std::string delimiter = "/";
	if (_path[_path.length() - 1] == '/') {
		delimiter = "";
	}

	if (otherPath[0] == '/') {
		return Path(_path + delimiter + otherPath.substr(1));
	}
	return Path(_path + delimiter + otherPath);
}

/**
 * @brief Prepend another path to the current path object and return a new path object.
*/
Path Path::prepend(Path& other) const {
	if (other.getType() != DIRECTORY) {
		throw InvalidOperationException(other.getPath() + " is not a directory");
	}
	
	const std::string& otherPath = other.getPath();

	std::string delimiter = "/";
	if (otherPath[otherPath.length() - 1] == '/') {
		delimiter = "";
	}

	if (_path[0] == '/') {
		return Path(otherPath + delimiter + _path.substr(1));
	}
	return Path(otherPath + delimiter + _path);
}

/**
 * @brief Prepend another path in the form of string and type to the current path object
 * and return the path object.
*/
Path Path::prepend(std::string otherPath, enum pathType type) const {
	Path other(otherPath, type);
	return prepend(other);
}

bool Path::operator==(const Path& other) {
	return (_path == other.getPath() && _type == other.getType());
}

bool Path::operator!=(const Path& other) {
	return !(*this == other);
}


/**
 * @param requestUri: the request uri
 * @param uriPrefix: the uri prefix
 * @param root: the root mapped for the uriPrefix
 * 
 * @brief Map the url to the corresponding file/directory on the filesystem.
 * 
 * @example
 * 
 * Args:
 * - requestUri: /image/cat/1.png
 * - uriPrefix: /image
 * - root: /var/www/html/public/images
 * Return:
 * - Path: /var/www/html/public/images/cat/1.png
 * 
 * Explain:
 * /image is mapped to /var/www/html/public/images,
 * then the postfix (/cat/1.png) is concatenated to the mapped path,
 * therefore became /var/www/html/public/images/cat/1.png
 *
 * 
 * @return Path - the mapped filesystem path
*/
Path Path::mapURLToFS(const Path& requestUri, const Path& uriPrefix, const Path& root, bool isCustomRoot) {
	const std::string& uriRef = requestUri.getPath();
	const std::string& uriPrefixRef = uriPrefix.getPath();

	if (isCustomRoot == false) {
		return root.concat(uriRef);
	}

	if (uriRef.substr(0, uriPrefixRef.length()) != uriPrefixRef) {
		throw InvalidOperationException("URI \"" + uriRef + "\" does not belong to uriPrefix \"" + uriPrefixRef + '"');
	}

	size_t postfixStart = uriPrefixRef.length();
	return root.concat(uriRef.substr(postfixStart));
}

std::string Path::read(void) const {
	std::ifstream infile(_path.c_str(), std::ios_base::in);

	if (!infile.is_open()) {
		return "";
	}

	std::stringstream buffer;
	buffer << infile.rdbuf();

	infile.close();

	return buffer.str();
}


void Path::write(std::string text) {
	std::ofstream outputFile(_path.c_str(), std::ios::out | std::ios::trunc);

    if (!outputFile.is_open()) {
        throw Path::InvalidPathException("File " + _path + " cannot be opened");
    }
    
    outputFile << text;

    outputFile.close();
}

std::string Path::getFileExtension(void) const {
	size_t extentionIndex = _path.find_last_of('.');
	if (extentionIndex == std::string::npos) {
		return "";
	}
	return _path.substr(extentionIndex);
}

Path Path::getDirectory(void) const {
	size_t dirDelimiter = _path.find_last_of('/');
	if (dirDelimiter == std::string::npos) {
		return Path(".");
	}

	return Path(_path.substr(0, dirDelimiter), DIRECTORY);
}

std::string Path::getFilename(void) const {
	size_t dirDelimiter = _path.find_last_of('/');
	if (dirDelimiter == std::string::npos) {
		return _path;
	}

	return _path.substr(dirDelimiter + 1);
}

Path& Path::operator<<(std::string text) {
	std::ofstream outputFile(_path.c_str(), std::ios::app);

    if (!outputFile.is_open()) {
        throw Path::InvalidPathException("File " + _path + " cannot be opened");
    }
    
    outputFile << text;

    outputFile.close();

	return *this;
}
