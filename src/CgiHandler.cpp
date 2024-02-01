/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 18:09:24 by wxuerui           #+#    #+#             */
/*   Updated: 2024/02/01 21:51:56 by wxuerui          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::~CgiHandler() {}

// Method to handle CGI execution
std::string CgiHandler::handleCgi(Request &request, Server &server, Location &location) {
	// Getting the absolute path of (e.g. ./www/upload/upload.py) - considering the best fit location block
    Path cgiPath = Path::mapURLToFS(request.getURI(), location.uri, location.root, location.isCustomRoot);
    cgiPath = wsutils::getRealPath(cgiPath.getPath());
    if (cgiPath.getType() == DIRECTORY) {
        // if the CGI script is the index of the directory, find it
        cgiPath = Response::find_default_index(cgiPath, &location);
    }

    std::string cgiHandler = server.cgiHandlers[cgiPath.getFileExtension()];

    if (cgiHandler != "" && cgiPath.isExecutable() == false) {
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
            
            // Execute CGI script
            char **envp = setEnv(request, location, cgiPath);
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
            std::string cgiBody = parseCgiOutput(pipefd_output[0], pipefd_stderror[0]);
            close(pipefd_output[0]);
            close(pipefd_stderror[0]);

            // Wait for the child process to finish
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 3) {
				// ERROR: Assume full HTTP error from parseCgiOutput above
				ret = cgiBody;
            } else if (cgiBody.substr(0,8) != "HTTP/1.1") {
				// SUCCESS: Without full HTTP header
				std::string output;
				output += std::string("HTTP/1.1") + " 200 OK\r\n";
				output += "Content-Type: text/html\r\n";
				output += "Content-Length: " + wsutils::toString(cgiBody.length()) + "\r\n";
				output += "\r\n";
				output += cgiBody;
				ret = output;
			}
			else
				// SUCCESS: With full HTTP header
				ret = cgiBody;
        }
    }
    return (ret);
}

char **CgiHandler::setEnv(Request& request, Location& location, Path& cgiPath) {
    std::vector<std::string> customEnvp;
    customEnvp.push_back("REQUEST_METHOD=" + request.getMethod());
    customEnvp.push_back("ROUTE=" + request.getURI().getPath());
    customEnvp.push_back("QUERY_STRING=" + request.getQueryParams());
    customEnvp.push_back("REDIRECT_STATUS=200");
    customEnvp.push_back("DOCUMENT_ROOT=" + cgiPath.getDirectory().getPath());
    customEnvp.push_back("SCRIPT_FILENAME=" + cgiPath.getPath());
    customEnvp.push_back("SERVER_PROTOCOL=HTTP/1.1");
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
    return (envp);
}

std::string	CgiHandler::parseCgiOutput(int pipefd_output, int pipefd_error) {
	char buffer[COMMON_BUFFER_SIZE];
	std::string output;
	int rec_byte;

	while ((rec_byte = read(pipefd_output, buffer, COMMON_BUFFER_SIZE)) != 0)
		output.append(buffer, rec_byte);
	while ((rec_byte = read(pipefd_error, buffer, COMMON_BUFFER_SIZE)) != 0)
		output.append(buffer,rec_byte);
	return (output);
}