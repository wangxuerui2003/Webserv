#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include "Request.hpp"
#include "Response.hpp"
#include "Parser.hpp"

class CgiHandler {
public:
    // Constructor
    CgiHandler();

    // Destructor
    ~CgiHandler();

    // Method to handle CGI execution
    std::string handleCgi(Request &request, Server &server, Location &location);
    std::string	parseCgiOutput(int pipefd_output, int pipefd_error);

private:
    // Private helper methods for CGI handling
    char **setEnv(Request &request);
};

#endif
