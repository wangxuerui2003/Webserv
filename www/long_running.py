#!/usr/bin/python3

import cgi
import os
import sys
from urllib.parse import unquote
import cgitb
cgitb.enable()

from time import sleep

sleep(10)

html_content = '''<!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Sleep done</title>
        </head>
        <body>
            <h1>Sleep Done</h1>
        </body>
        </html>
        '''

print(html_content)