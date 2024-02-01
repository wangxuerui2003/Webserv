/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 17:48:28 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/01 12:05:44 by zwong            ###   ########.fr       */
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
	wsutils::log("decoded string is: " + decoded.str(), "./logs");
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

	// Change path type to it's real type if expected type is wrong
	if (_type != URI && realPathType != _type) {
		throw InvalidOperationException("Expected file type of " + path + " does not match the real file type");
	}

	if (_type != URI && isAccessible(_path.c_str()) == false) {
		throw InvalidPathException(path + " is not accessible");
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
Path Path::concat(Path& other) {
	return concat(other.getPath(), other.getType());
}

/**
 * @brief Concatenate another path in the form of string and type to the current path object
 * and return a new path object.
*/
Path Path::concat(std::string otherPath, enum pathType type) {
	if (_type != DIRECTORY) {
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
Path Path::concat(std::string otherPath) {
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
Path Path::prepend(Path& other) {
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
Path Path::prepend(std::string otherPath, enum pathType type) {
	Path other(otherPath, type);
	return prepend(other);
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
Path Path::mapURLToFS(Path& requestUri, Path& uriPrefix, Path& root, bool isCustomRoot) {
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

/**
 * @param locations: a vector of location patterns/ uri prefix
 * @param requestUri: the request uri
 * 
 * @brief Get the best fit location that the request URI is requesting for.
 * 
 * @return Path * - the path pointer to the best fit uri prefix. NULL for no match
*/
Location *Path::getBestFitLocation(std::vector<Location>& locations, Path& requestUri) {
	Location *bestFit = NULL;
	size_t layersMatched = 0;

	for (size_t i = 0; i < locations.size(); ++i) {
		const std::string& uriRef = requestUri.getPath();
		const std::string& uriPrefixRef = locations[i].uri.getPath();
		size_t start = 0;
		size_t layersMatchedTemp = 0;
		
		while (start < uriPrefixRef.length()) {
			size_t uriDelimiter = uriRef.find('/', start);
			if (uriDelimiter == std::string::npos) {
				uriDelimiter = uriRef.length();
			}
			
			size_t uriPrefixDelimiter = uriPrefixRef.find('/', start);
			if (uriPrefixDelimiter == std::string::npos) {
				uriPrefixDelimiter = uriPrefixRef.length();
			}

			// The position of the next / doesn't match, wrong location
			if (uriDelimiter != uriPrefixDelimiter) {
				layersMatchedTemp = 0;
				break;
			}

			// The segment of uri and uriPrefix doesn't match, wrong location
			if (uriRef.substr(start, uriDelimiter - start) != uriPrefixRef.substr(start, uriPrefixDelimiter - start)) {
				layersMatchedTemp = 0;
				break;
			}

			start = uriDelimiter + 1;
			layersMatchedTemp++;
		}

		if (layersMatchedTemp > layersMatched) {
			layersMatched = layersMatchedTemp;
			bestFit = &(locations[i]);
		}
	}

	return bestFit;
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

void Path::write(std::string filePath, std::string content) {
	std::ofstream outputFile(filePath.c_str(), std::ios::out | std::ios::trunc);

    if (!outputFile.is_open()) {
        throw Path::InvalidPathException("File " + filePath + " cannot be opened");
    }
    
    outputFile << content;

    outputFile.close();
}

std::string Path::generateDirectoryListing(void) const {
	if (_type != DIRECTORY)
		throw InvalidOperationException("File " + _path + " is not a directory");
    std::string html = "<html>\n<head><title>Index of " + _path + "</title></head>\n<body>\n<h1>Index of " + _path + "</h1>\n<ul>\n";

    DIR* dir = opendir(_path.c_str());
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            const std::string entryName = entry->d_name;
            html += "<li><a href=\"" + entryName + "\">" + entryName + "</a></li>\n";
        }
        closedir(dir);
    }

    html += "</ul>\n</body>\n</html>\n";

	std::string response;

	response += "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/html\r\n";
	response += ("Content-Length: " + wsutils::toString<size_t>(html.length()) + "\r\n");
	response += "\r\n";
	response += html;
	return (response);
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
		throw InvalidOperationException("No Parent directory found for" + _path);
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

