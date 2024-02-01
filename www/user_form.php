#!/usr/bin/php-cgi
<?php
if ($_SERVER["REQUEST_METHOD"] === "POST") {
    // POST request: Display the submitted data
    $name = $_POST["name"];
    $age = $_POST["age"];
    echo "<html><body>";
    echo "<h1>Submitted Data</h1>";
    echo "<p>Name: $name</p>";
    echo "<p>Age: $age</p>";
    echo "</body></html>";
} else {
    // GET request: Display the input form
    echo "<html><body>";
    echo "<h1>Enter Your Information</h1>";
    echo '<form method="post" action="' . $_SERVER["PHP_SELF"] . '">';
    echo '  <label for="name">Name:</label>';
    echo '  <input type="text" id="name" name="name" required><br>';
    echo '  <label for="age">Age:</label>';
    echo '  <input type="number" id="age" name="age" required><br>';
    echo '  <input type="submit" value="Submit">';
    echo '</form>';
    echo "</body></html>";
}
?>