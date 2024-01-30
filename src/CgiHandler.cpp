#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::~CgiHandler() {}

// Method to handle CGI execution
std::string CgiHandler::handleCgi(Request &request, Server &server, Location &location) {
    // CGI handling logic implementation
    // TODO: Parse CGI path
    // std::string cgiPath = "." + location.cgi_pass.getPath(); // make sure the path has "./"
	std::string cgiPath = Path::mapURLToFS(request.getPath(), location.uri, location.root).getPath();
	// char *argv[2] = {(char *)cgiPath.c_str(), NULL};

	// wsutils::log(request.getBody(), std::cerr);

	char **argv = NULL;
    
    int pid1;
	int pid2;
	int i = 2;
	int pipefd_input[2];
	int pipefd_output[2];
	int pipefd_stderror[2];
    std::string ret;

	if (pipe(pipefd_input) == -1 || pipe(pipefd_output) == -1 || pipe(pipefd_stderror) == -1)
		ret = Response::parse_error_pages("500", "Internal Server Error", server);
	else {
		// this part is just copying body from parent to CGI process
		while (i > 0) {
			if (i == 2 && (pid1 = fork())==0) {
				dup2(pipefd_input[1], STDOUT_FILENO);
				close(pipefd_input[0]);
				close(pipefd_input[1]);
				close(pipefd_output[1]);
				close(pipefd_output[1]);
				close(pipefd_stderror[0]);
				close(pipefd_stderror[1]);
                wsutils::log("Request body length is: " + std::to_string(request.getBody().length()), "./logs");
				write(STDOUT_FILENO, request.getBody().c_str(), request.getBody().length());
				exit(0);
			}
			else if (i == 1 && (pid2=fork())==0) {
				dup2(pipefd_input[0], STDIN_FILENO);
				close(pipefd_input[0]);
				close(pipefd_input[1]);
				dup2(pipefd_output[1], STDOUT_FILENO);
				close(pipefd_output[1]);
				close(pipefd_output[0]);
				dup2(pipefd_stderror[1], STDERR_FILENO);
				close(pipefd_stderror[0]);
				close(pipefd_stderror[1]);
				char **envp = setEnv(request);
				if (execve(cgiPath.c_str(), argv, envp) == -1) {
					ret = Response::parse_error_pages("500", "Internal Server Error", server);
					write(STDOUT_FILENO, ret.c_str(), ret.length());
					exit(3);
				}
			}
			i--;
		}
		close(pipefd_input[1]);
		close(pipefd_output[1]);
		close(pipefd_stderror[1]);
		int exit_status;

		std::string cgiBody;
		cgiBody = parseCgiOutput(pipefd_output[0], pipefd_stderror[0]);
        wsutils::log("CGI OUTPUT IS:\n" + cgiBody, "./logs");
		waitpid(pid2, &exit_status, 0);
		if (WIFEXITED(exit_status) && WEXITSTATUS(exit_status) == 3) {
            ret = cgiBody;
		}
		else {
			if (request.getMethod() == "POST") {
				if (cgiBody.substr(0,8) != "HTTP/1.1") {
					std::string output;
					output += std::string("HTTP/1.1") + " 200 OK\r\n";
					output += "Content-Type: text/html\r\n";
					output += "Content-Length: " + std::to_string(cgiBody.length()) + "\r\n";
					output += "\r\n";
					output += cgiBody;
                    ret = output;
				}
				else
                    ret = cgiBody;
			}
			else if (request.getMethod() == "GET") {
				if (cgiBody.substr(0,8) != "HTTP/1.1") {
					std::string output;
					output += std::string("HTTP/1.1") + " 200 OK\r\n";
					output += "Content-Type: text/html\r\n";
					output += "Content-Length: " + std::to_string(cgiBody.length()) + "\r\n";
					output += "\r\n";
					output += cgiBody;
                    ret = output;
				}
				else
                    ret = cgiBody;
			}
		}
		close(pipefd_input[0]);
		close(pipefd_output[0]);
		close(pipefd_stderror[0]);
	}
    return (ret);
}

char **CgiHandler::setEnv(Request &request) {
    // std::vector<std::string> variables;

    // Add relevant environment variables based on request and server information
    // variables.push_back("REQUEST_METHOD=" + request.getMethod());
    // variables.push_back("CONTENT_TYPE=" + request.getHeader("Content-Type"));
    // variables.push_back("ROUTE=" + request.getPath().getPath());
    // // TODO: Add route?

    // // Convert vector of strings to array of C-style strings
    // char **envp = new char*[variables.size() + 1];
    // for (size_t i = 0; i < variables.size(); ++i) {
    //     envp[i] = strdup(variables[i].c_str());
    //     wsutils::log(envp[i], "./logs");
    // }
    // envp[variables.size()] = NULL; // Null-terminate the array
    // return (envp);

    std::map<std::string, std::string>::iterator it = request.getHeaderMap().begin();
	std::map<std::string, std::string>::iterator ite = request.getHeaderMap().end();
	char **envp = (char **)malloc((request.getHeaderMap().size() + 4) * sizeof(char *));
	
	int i = 0;
	envp[i] = strdup((std::string("REQUEST_METHOD=") + request.getMethod()).c_str());
	i++;
	envp[i] = strdup((std::string("ROUTE=") + request.getPath().getPath()).c_str());
	i++;
	envp[i] = strdup((std::string("QUERY_STRING=") + request.getQueryParams()).c_str());
	i++;
	for (; it != ite; it++) {
		std::string field = it->first;
		size_t pos = field.find_first_of("-");
		while(pos != std::string::npos) {
			field.replace(pos, 1, "_");
			pos = field.find_first_of("-");
		}
		std::transform(field.begin(), field.end(), field.begin(), ::toupper);
		envp[i] = strdup((char *)std::string(field + "=" + it->second).c_str());
		setenv(field.c_str(),it->second.c_str(), 1);
		i++;
	}
	envp[i] = NULL;

    // wsutils::log("ENVP IS: ", "./logs");
    // i = 0;
    // while (envp[i])
    //     wsutils::log(envp[i++], "./logs");
	return (envp);
}

std::string	CgiHandler::parseCgiOutput(int pipefd_output, int pipefd_error) {
	char buffer[1024];
	std::string output;
	int rec_byte;

	while ((rec_byte = read(pipefd_output, buffer, 1024)) != 0)
		output.append(buffer, rec_byte);
	while ((rec_byte = read(pipefd_error, buffer, 1024)) != 0)
		output.append(buffer,rec_byte);
	return (output);
}