<?php
if ($_SERVER["REQUEST_METHOD"] === "POST") {
	// send back the POSTed session data to webserv with a X-Replace-Session header
	// webserv will then store the session data into a csv file and Set-Cookie for the client
    $name = $_POST["name"];
    $intra = $_POST["intra-id"];
    header('X-Replace-Session: ' . "name=$name;intra=$intra");
    header('Location: /stateful_site');
    die();
} else {
	// GET request: Display the get_session form
	// Take a form of name and intra-id
    echo "<html><body>";
    echo "<h1>Enter Your Information</h1>";
    echo '<form method="post" action="' . $_SERVER["PHP_SELF"] . '">';
    echo '  <label for="name">Name:</label>';
    echo '  <input type="text" id="name" name="name" placeholder="Name" required><br>';
	echo '  <label for="intra-id">Intra ID:</label>';
    echo '  <input type="text" id="intra-id" name="intra-id" placeholder="Intra ID" required><br>';
    echo '  <input type="submit" value="Submit">';
    echo '</form>';
    echo "</body></html>";
}
?>