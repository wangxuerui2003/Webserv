#!/usr/bin/python3

import cgi
import os
import sys
from urllib.parse import unquote
import cgitb
cgitb.enable()

# PROBLEM: form is empty, not parsed properly from html form WHY?? hmm..
request_method = os.environ.get("REQUEST_METHOD").upper()
route = unquote("." + os.environ.get("ROUTE"))

if request_method == "POST":
	form = cgi.FieldStorage()
	print(form)
	if "file" in form and form["file"].filename:
		file_item = form["file"]
		current_dir = os.path.dirname(os.path.abspath(__file__))
		filename = os.path.join(current_dir, file_item.filename)
		with open(filename, "wb") as file:
			file.write(file_item.file.read())
		print("<html><body style='font-family: Arial; sans-serif;margin: 20px;'>")
		print("<div>File uploaded successfully.</div>")
		print("<div>Uploaded File: {}</div>".format(filename))
		print("</body></html>")
	else:
		print("No file uploaded.")

elif request_method == "GET":
	html_content = '''
	<!DOCTYPE html>
	<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<title>File Upload</title>
	</head>
	<body>
		<h2>Upload File</h2>
		<form action="/upload/upload.py" method="post" enctype="multipart/form-data">
			<input type="file" name="file" id="file"><br><br>
			<input type="submit" value="Upload File">
		</form>
	</body>
	</html>
	'''

	print(html_content)
