# **Webserv**

### **Overview**
**Webserv** is an Nginx-like HTTP Server based on C++98. Webserv can be used to serve multiple complete static website through the ***virtual hosting*** technology. This is the second-last project of the *42 Core Program*.

<br>

**Platform**
<br>
Linux, MacOS/FreeBSD

### Features
- Non-Blocking I/O network operations based on select/poll
- Supports HTTP/1.1
- Supports defining HTTP redirection routes in config file
- CGI: python-cgi, php-cgi
- Cookies and Session Management

### Preparation
```bash
# Auto generate a local config file
$ ./configure
```

### Usage
```bash
$ make
$ ./webserv <config-file>  # if no config provided, default will be ./conf/local.conf
```

### Configuration File
```bash
# A simple example of a configuration file
event {
	event_handler poll;  # supports select and poll
}

server {
	listen 127.0.0.1:8080;
	root /var/www/html;
	index index.html index.htm;
	server_name localhost example.com;  # virtual hosting

	session_store /var/www/html/sessions.csv;  # the session data "database"
	session_expire_seconds 3600;

	cgi_handler .py;  # auto-detect interpreter
	cgi_handler .php /usr/bin/php-cgi;

	location / {
		# allow GET method only by default
	}

	location /public {
		autoindex on;  # directory listing
	}

	location /temp_moved {
		return 302 /public;  # http redirection
	}

	location /stateful_site {
		index index.php;
		limit_except GET POST;  # allows GET and POST
	}

	location /upload {
		limit_except GET POST;
		index upload.py;
		client_max_body_size 20M;  # max file size
		upload_store /uploads;  # relative upload path
	}
}
```

### Cookies and Session Management
```php
<?php
	// Example for retrieving session data in php-cgi
	$session_data = $_SERVER["WEBSERV_SESSION_DATA"];
	echo "<html><body>";
	echo $session_data;
	echo "</body></html>";
?>

<?php
	// Example for creating new session in php-cgi
	$name = $_POST["name"];
    $intra = $_POST["intra-id"];
    header('X-Replace-Session: ' . "name=$name;intra=$intra");
?>
```

### The Team
- [Xue Rui](https://github.com/wangxuerui2003)
- [Zheng Xiang](https://github.com/Wongoose)
- [Mark Liew](https://github.com/Cloonie)