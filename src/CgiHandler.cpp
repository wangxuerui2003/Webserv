#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::~CgiHandler() {}

// Method to handle CGI execution
std::string CgiHandler::handleCgi(Request &request, Server &server, Location &location) {
    // CGI handling logic implementation
    // TODO: Parse CGI path
    std::string cgiPath = location.cgi_pass.getPath() + "/upload.py"; // make sure the path has "./"
    std::string cgiDir = "./cgi/";
    wsutils::log("Setting CGI path is:\n" + cgiPath, "./logs");
    wsutils::log("Setting CGI directory is:\n" + cgiDir, "./logs");
	// std::string cgiDir = cgiPath.substr(0, cgiPath.find_first_of("/", 2) + 1);
	char *argv[3] = {(char *)cgiPath.c_str(), (char *)cgiDir.c_str(), NULL};
    char **envp = setEnv(request);
    // Execute CGI script and process output
    // Implement the CGI execution logic using pipes and other necessary operations

    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        wsutils::log("PIPE FAILED", "./logs");
        return (Response::parse_error_pages("501", "Internal pipe error", server));
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        wsutils::log("FORK FAILED", "./logs");
        return (Response::parse_error_pages("501", "Internal fork error", server));
    }

    if (pid == 0) { // Child process (CGI script)
        wsutils::log("CGI running in child process...", "./logs");
        // Close the write end of the pipe
        close(pipefd[1]);

        // Redirect stdin to the read end of the pipe
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        // Execute the CGI script
        execve(cgiPath.c_str(), argv, envp);
        
        // If execl fails, print an error
        perror("execve");
        wsutils::log("EXECVE FAILED", "./logs");
        return (Response::parse_error_pages("501", "Internal execve error", server));
    } else { // Parent process (Web server)
        // Close the read end of the pipe
        close(pipefd[0]);

        // Write data to the CGI script through the pipe
        wsutils::log("SENDING DATA FROM PARENT:\n" + request.getBody(), "./logs");
        write(pipefd[1], request.getBody().c_str(), strlen(request.getBody().c_str()));
        close(pipefd[1]); // Close the write end to signal end of data

        // Read the output from the CGI script
        char buffer[4096];
        ssize_t bytesRead;
        std::string cgiOutput;
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            cgiOutput.append(buffer, bytesRead);
        }

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
        wsutils::log("FINISH CHILD PROCESS", "./logs");

        if (WIFEXITED(status)) {
            std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
        } else {
            std::cout << "Child process terminated abnormally" << std::endl;
            return (Response::parse_error_pages("501", "Internal process error", server));
        }

        // Print the output from the CGI script
        wsutils::log("Output from CGi script:\n" + cgiOutput + "\n\nEND", "./logs");
    }

    return (0);
}

char **CgiHandler::setEnv(Request &request) {
    wsutils::log("SETTING ENVP", "./logs");
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
	char **envp = (char **)malloc((request.getHeaderMap().size() + 3) * sizeof(char *));
	
	int i = 0;
	envp[i] = strdup((std::string("REQUEST_METHOD=") + request.getMethod()).c_str());
	i++;
	envp[i] = strdup((std::string("ROUTE=") + request.getPath().getPath()).c_str());
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
	return (envp);
}