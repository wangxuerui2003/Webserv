<?php
// If have session WEBSERV_SESSION_DATA, display the session data.
if (array_key_exists("WEBSERV_SESSION_DATA", $_SERVER)) {
	$session_data = $_SERVER["WEBSERV_SESSION_DATA"];
	$name_value_pairs_array = explode(';', $session_data);
	echo "<html><body>";
	foreach ($name_value_pairs_array as $name_value_pair) {
		[$name, $value] = explode('=', $name_value_pair);
		echo "<h1>Your $name is: $value</h1>";
	}
	echo "</body></html>";
} else {
	// don't have session yet, redirect to get_session.php
	header('Location: ' . '/stateful_site/get_session.php');
	die();
}
?>