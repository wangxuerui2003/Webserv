<?php
// TODO: If have session WEBSERV_SESSION_DATA, display the session data.
// TODO: If don't have session, redirect to get_session.php
if (array_key_exists("WEBSERV_SESSION_DATA", $_SERVER)) {
	$data = $_SERVER["WEBSERV_SESSION_DATA"];
	echo "<html><body>";
	echo "<h1>$data</h1>";
	echo "</body></html>";
} else {
	print "X-Replace-Session: test1=wangxuerui;\r\n";
}
?>