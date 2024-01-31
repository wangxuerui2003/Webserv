#!/usr/bin/python3

import sys
import os

request_method = os.environ.get("REQUEST_METHOD").upper()

# Function to parse the unchunked request body
def parse_unchunked_request_body():
    # Read the unchunked request body from stdin
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    unchunked_request_body = sys.stdin.read(content_length)
    return unchunked_request_body

# Main CGI logic
if request_method == "POST":
	unchunked_request_body = parse_unchunked_request_body()
	print(unchunked_request_body)
