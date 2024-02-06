/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 18:09:24 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/06 21:33:28 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

// Method to handle CGI execution
std::string CgiHandler::handleCgi(Request &request, Server &server, Location &location) {
	// Getting the absolute path of (e.g. ./www/upload/upload.py) - considering the best fit location block
    Path cgiPath = Path::mapURLToFS(request.getURI(), location.uri, location.root, location.isCustomRoot);
    if (cgiPath.getType() == DIRECTORY) {
        // if the CGI script is the index of the directory, find it
        cgiPath = Response::find_default_index(cgiPath, &location);
    }

    std::string cgiHandler = server.cgiHandlers[cgiPath.getFileExtension()];

    if (cgiHandler == "" && cgiPath.isExecutable() == false) {
        return Response::parse_error_pages("500", "CGI script is not executable", server);
    }

    int pipefd_input[2];
    int pipefd_output[2];
    int pipefd_stderror[2];
    std::string ret;

    if (pipe(pipefd_input) == -1 || pipe(pipefd_output) == -1 || pipe(pipefd_stderror) == -1) {
        ret = Response::parse_error_pages("500", "Internal Server Error", server);
    } else {
        pid_t pid;
        if ((pid = fork()) == -1) {
            ret = Response::parse_error_pages("500", "Internal Server Error", server);
        } else if (pid == 0) { // Child process
            close(pipefd_input[1]);
            close(pipefd_output[0]);
            close(pipefd_stderror[0]);

            // Redirect stdin, stdout, and stderr
            dup2(pipefd_input[0], STDIN_FILENO);
            dup2(pipefd_output[1], STDOUT_FILENO);
            dup2(pipefd_stderror[1], STDERR_FILENO);
            
            // Execute CGI script
            char **envp = setEnv(request, server, location, cgiPath);

            // Let CGI script current directory have the correct relative path access
            std::string cgiDir = cgiPath.getDirectory().getPath();
            chdir(cgiDir.c_str());

            std::string cgiRelativePath = cgiPath.getFilename();

            if (cgiHandler == "") {
                // if CGI has no interpreter, the file itself becomes the handler/first arg for execve()
                cgiHandler = cgiRelativePath;
            }

            char *argv[] = {
                const_cast<char *>(cgiHandler.c_str()),
                const_cast<char *>(cgiRelativePath.c_str()),
                NULL
            };

            if (execve(argv[0], argv, envp) == -1) {
				ret = Response::parse_error_pages("500", "Internal Server Error execve", server);
				write(STDOUT_FILENO, ret.c_str(), ret.length());
                exit(3);
            }
        } else { // Parent process
            close(pipefd_input[0]);
            close(pipefd_output[1]);
            close(pipefd_stderror[1]);

            // Write request body to the input pipe
            write(pipefd_input[1], request.getBody().c_str(), request.getBody().length());
            close(pipefd_input[1]);

            // Read CGI output from the output pipe
            std::string cgiOutput = parseCgiOutput(pipefd_output[0], pipefd_stderror[0]);
            close(pipefd_output[0]);
            close(pipefd_stderror[0]);

            // Wait for the child process to finish
            int status;
            waitpid(pid, &status, 0);
			if (WIFEXITED(status) && WEXITSTATUS(status) == 3) {
				// ERROR: Assume full HTTP error from parseCgiOutput above
                return cgiOutput;
            }
            
            std::string output;
            output += std::string("HTTP/1.1") + " 200 OK\r\n";
            size_t cgiHeaderTerminator = cgiOutput.find(HTTP_HEADER_TERMINATOR);

            // if no header found, add a default header for the CGI output
            if (cgiHeaderTerminator == std::string::npos) {
                std::cout << "No header" << std::endl;
                output += "Content-Type: text/html\r\n";
                output += "Content-Length: " + wsutils::toString(cgiOutput.length()) + "\r\n";
                output += "\r\n";
                return output + cgiOutput;
            }

            // Found header, parse the header and extract necessary information, then return the response string
            std::string sessionData = parseXReplaceSession(cgiOutput);
            std::string cgiHeader = cgiOutput.substr(0, cgiHeaderTerminator);
            std::string cgiBody = cgiOutput.substr(cgiHeaderTerminator + 4);
            cgiHeader += "\r\nContent-Length: " + wsutils::toString(cgiBody.length()) + "\r\n";
            if (sessionData != "") {
                std::string sessionId = server.session.addNewSession(sessionData, server.sessionExpireSeconds);
                cgiHeader += setCookie(sessionId, server.sessionExpireSeconds);
            }
            cgiHeader += "\r\n";
            return output + cgiHeader + cgiBody;
        }
    }
    return (ret);
}

std::string CgiHandler::setCookie(std::string& sessionId, time_t expireAfterSeconds) {
    std::time_t expirationTimestamp = std::time(NULL) + expireAfterSeconds;

    const char* format = "%a, %d-%b-%Y %H:%M:%S GMT";

    // Buffer to store the formatted time string
    char timeString[100];

    // Format the time string
    std::strftime(timeString, sizeof(timeString), format, std::gmtime(&expirationTimestamp));

    return "Set-Cookie: " WEBSERV_SESSION_ID_NAME "=" + sessionId + ';' + " expires=" + timeString + "\r\n";
}

std::string CgiHandler::parseXReplaceSession(std::string& cgiOutput) {
    size_t sessionDataHeader = cgiOutput.find("X-Replace-Session: ");
    if (sessionDataHeader == std::string::npos) {
        return "";
    }

    size_t start = sessionDataHeader + std::strlen("X-Replace-Session: ");
    size_t end = cgiOutput.find("\r\n", sessionDataHeader);
    if (end == std::string::npos) {
        throw std::runtime_error("Incorrect CGI output structure");
    }

    std::string sessionData = cgiOutput.substr(start, end - start);
    cgiOutput.erase(sessionDataHeader, end - sessionDataHeader + 2);

    return sessionData;
}

char **CgiHandler::setEnv(Request& request, Server& server, Location& location, Path& cgiPath) {
    std::vector<std::string> customEnvp;
    customEnvp.push_back("REQUEST_METHOD=" + request.getMethod());
    customEnvp.push_back("ROUTE=" + request.getURI().getPath());
    customEnvp.push_back("QUERY_STRING=" + request.getQueryParams());
    customEnvp.push_back("REDIRECT_STATUS=200");
    customEnvp.push_back("DOCUMENT_ROOT=" + cgiPath.getDirectory().getPath());
    customEnvp.push_back("SCRIPT_FILENAME=" + cgiPath.getPath());
    customEnvp.push_back("SERVER_PROTOCOL=HTTP/1.1");
    // Special case HTTP envs
    customEnvp.push_back("HTTP_USER_AGENT=" + request.getHeader("User-Agent"));
    customEnvp.push_back("HTTP_ACCEPT=" + request.getHeader("Accept"));
    customEnvp.push_back("HTTP_REFERER=" + request.getHeader("Referer"));

    if (server.hasSessionManagement) {
        std::string sessionId = request.getCookieByName(WEBSERV_SESSION_ID_NAME);
        if (sessionId != "") {
            customEnvp.push_back(std::string(SESSION_DATA_CGI_HEADER) + "=" + server.session.getSessionDataById(sessionId));
        }
    }

    if (location.accept_upload == true) {
        customEnvp.push_back("UPLOAD_STORE=" + location.upload_store.getPath());
    }
    
    char **envp = new char*[request.getHeaderMap().size() + customEnvp.size() + 1];
    size_t i = 0;
    for (; i < customEnvp.size(); ++i) {
        envp[i] = const_cast<char *>(strdup(customEnvp[i].c_str()));
    }

    // Add other header fields
    std::map<std::string, std::string>::iterator it = request.getHeaderMap().begin();
    std::map<std::string, std::string>::iterator ite = request.getHeaderMap().end();
    for (; it != ite; ++it) {
        std::string field = it->first;
        size_t pos = field.find_first_of("-");
        while (pos != std::string::npos) {
            field.replace(pos, 1, "_");
            pos = field.find_first_of("-");
        }
		// Transform the env keys to upper case (fields)
        std::transform(field.begin(), field.end(), field.begin(), ::toupper);
		// Adds the values to each UPPERCASE FIELD
        envp[i] = strdup((field + "=" + it->second).c_str());
		// Optional setenv() in the current process (just for safe guard)
        setenv(field.c_str(), it->second.c_str(), 1);
        i++;
    }
    envp[i] = NULL;
    wsutils::log("ENVP:\n", "./logs");
    int j = 0;
    while (envp[j])
        wsutils::log(envp[j++], "./logs");
    return (envp);
}

std::string	CgiHandler::parseCgiOutput(int pipefd_output, int pipefd_error) {
	char buffer[READ_BUFFER_SIZE];
	std::string output;
	int rec_byte;

	while ((rec_byte = read(pipefd_output, buffer, READ_BUFFER_SIZE)) != 0)
		output.append(buffer, rec_byte);
	while ((rec_byte = read(pipefd_error, buffer, READ_BUFFER_SIZE)) != 0)
		output.append(buffer,rec_byte);

	return (output);
}
