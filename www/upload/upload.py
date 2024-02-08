#!/usr/bin/python3

import cgi
import os
import sys
from urllib.parse import unquote
import cgitb
cgitb.enable()

request_method = os.environ.get("REQUEST_METHOD").upper()
content_type = os.environ.get("CONTENT_TYPE", "")
route = unquote("." + os.environ.get("ROUTE"))


def get_file_object():
	form = cgi.FieldStorage()
	if "file" in form and form["file"].filename:
		return form["file"]
	else:
		print("<html><body style='font-family: Arial; sans-serif;margin: 20px;'>")
		print("<div>No file uploaded.</div>")
		print('<a href="/upload">Upload Page</a>')
		print("<br>")
		print('<a href="/">Home Page</a>')
		print("</body></html>")
		sys.exit(0)


def store_file(file_object):
	# Custom upload location
	current_dir = os.path.dirname(os.path.abspath(__file__))
	upload_store = os.environ.get("UPLOAD_STORE", "")
	file_upload_dir = os.path.join(current_dir, upload_store.lstrip('/'))

	# Create the upload_store directory if not exist
	if not os.path.exists(file_upload_dir):
		os.makedirs(file_upload_dir)

	filename = os.path.join(file_upload_dir, file_object.filename)
	with open(filename, "wb") as file:
		file.write(file_object.file.read())
	
	return filename


if request_method == "POST":
	if not content_type.startswith("multipart/form-data"):
		print("Content-Type: text/plain")
		print()
		print("File uploads are only allowed with multipart/form-data.")
		sys.exit(0)

	file_object = get_file_object()
	filename = store_file(file_object)

	print("<html><body style='font-family: Arial; sans-serif;margin: 20px;'>")
	print("<div>File uploaded successfully.</div>")
	print("<div>Uploaded File: {}</div>".format(filename))
	print('<a href="/upload">Upload Page</a>')
	print("<br>")
	print('<a href="/">Home Page</a>')
	print("</body></html>")
		

elif request_method == "GET":
	html_content = '''
	<!DOCTYPE html>
	<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<link rel="stylesheet" type="text/css" href="/css/styles.css">
		<title>Webserv Upload Page</title>
	</head>
	<body>
		<h1>WEBSERV</h1>
		<ul>
			<li><a href="/">Home</a></li>
			<li><a href="/upload/upload.py">Upload</a></li>
			<li><a href="/database.html">Database</a></li>
			<li><a href="/about.html">About</a></li>
		</ul>
		<div class="block">
			<h2>Upload File</h2>
			<form action="/upload/upload.py" method="post" enctype="multipart/form-data">
				<input type="file" name="file" id="file"><br><br>
				<input type="submit" value="Upload File">
			</form>
		</div>
	</body>
	</html>
	'''
	print(html_content)
