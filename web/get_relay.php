<?php
$servername = "localhost";
$username = "smartene_userfarid";
$password = "faridfadlu123";
$dbname = "smartene_listrik";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Get the ID_Listrik parameter from the URL
$ID_Listrik = $_GET['ID_Listrik'] ?? null;

if ($ID_Listrik) {
    // Prepare and bind
    $stmt = $conn->prepare("SELECT `stat` FROM `statusrelay` WHERE `ID_Listrik`= ? ORDER BY `id` DESC LIMIT 1");
    $stmt->bind_param("s", $ID_Listrik);

    // Execute the query
    $stmt->execute();

    // Bind result variables
    $stmt->bind_result($value);

    // Fetch the value
    if ($stmt->fetch()) {
        echo $value;
    } else {
        echo "Error: No data found";
    }

    // Close the statement and connection
    $stmt->close();
} else {
    echo "Error: ID_Listrik parameter is missing";
}

$conn->close();
?>
