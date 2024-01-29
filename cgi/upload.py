#!/usr/bin/env python

import cgi
import cgitb
import os

# Enable CGI error reporting for debugging
cgitb.enable()

# Set the upload directory
UPLOAD_DIR = "./www/public/uploads"

# Create the upload directory if it doesn't exist
if not os.path.exists(UPLOAD_DIR):
    os.makedirs(UPLOAD_DIR)

# Set the maximum file size allowed (in bytes)
MAX_FILE_SIZE = 10 * 1024 * 1024  # 10 MB

# HTML content type
print("Content-Type: text/html\n")

# HTML template for displaying upload status
html_template = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>File Upload Status</title>
</head>
<body>
    <h2>File Upload Status</h2>
    {}
</body>
</html>
"""

# Get form data
form = cgi.FieldStorage()

# Check if a file was uploaded
if "fileToUpload" in form:
    file_item = form["fileToUpload"]

    # Check file size
    if file_item.file:
        file_size = int(os.environ.get("CONTENT_LENGTH", 0))
        if file_size > MAX_FILE_SIZE:
            print(html_template.format("<p>File size exceeds the limit.</p>"))
            exit()

    # Save the file to the upload directory
    file_path = os.path.join(UPLOAD_DIR, os.path.basename(file_item.filename))
    with open(file_path, "wb") as outfile:
        while True:
            chunk = file_item.file.read(1024)
            if not chunk:
                break
            outfile.write(chunk)

    print(html_template.format("<p>File uploaded successfully: {}</p>".format(file_item.filename)))
else:
    print(html_template.format("<p>No file uploaded.</p>"))
