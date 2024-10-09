<?php
// Database connection details
$servername = "localhost";
$username = "smartene_userfarid";
$password = "faridfadlu123";
$dbname = "smartene_listrik";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

/// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Get ID_Listrik from the request
if (isset($_GET['ID_Listrik'])) {
    $ID_Listrik = $_GET['ID_Listrik'];

    // Prepare and execute SQL query
    $stmt = $conn->prepare("SELECT * FROM meter WHERE ID_Listrik = ?");
    $stmt->bind_param("s", $ID_Listrik);
    $stmt->execute();
    $result = $stmt->get_result();

    // Fetch data and output as JSON
    if ($result->num_rows > 0) {
        $data = $result->fetch_assoc();
        echo json_encode($data);
    } else {
        echo json_encode(array("error" => "No data found for ID_Listrik: $ID_Listrik"));
    }

    // Close statement
    $stmt->close();
} else {
    echo json_encode(array("error" => "ID_Listrik parameter is missing"));
}

// Close connection
$conn->close();
?>