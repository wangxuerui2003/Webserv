#!/usr/bin/python3

import cgi
import os
import sys
from urllib.parse import unquote
import cgitb
cgitb.enable()

request_method = os.environ.get("REQUEST_METHOD").upper()
route = unquote("." + os.environ.get("ROUTE"))

def output_html(custom_section):
    html_content = f'''
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>User Form</title>
        </head>
        <body>
            {custom_section}
        </body>
        </html>
        '''
	
    print(html_content)


if request_method == "GET":
	form = cgi.FieldStorage()
	
	if not form.list:
		output_html('''<h2>Enter Your Info</h2>
        <form action="/user_form.py" method="get">
            <input type="text" name="name" id="name" placeholder="Full Name"><br>
            <input type="number" name="age" id="age" placeholder="Age"><br>
            <input type="submit">
        </form>''')
	else:
		name_param = form.getvalue('name')
		age_param  = form.getvalue('age')
		output_html(f'''
            <h1>Hello! {name_param}, age {age_param}</h1>
        ''')
