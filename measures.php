<?php
include "config.php";

$temperature = isset($_GET['temperature']) ? (float)$_GET['temperature'] : NULL;
$rssi = isset($_GET['rssi']) ? (int)$_GET['rssi'] : NULL;

// Check si l'une des deux valeurs est null | les deux pipes siginifient OU | === pour vérifier la valeur ET le type
if ($temperature === null || $rssi === null) {
    http_response_code(400);
    die("Paramètres manquants");
}

if ($temperature > 50 || $temperature < -30 || $temperature == 4.04) {
    http_response_code(400);
    die("Valeurs incohérentes");
}

$stmt = $pdo->prepare("INSERT INTO meteo (temperature, rssi) VALUES (:temperature, :rssi)");
$stmt->execute([':temperature' => $temperature, ':rssi' => $rssi]);

echo "Valeurs enregistrées";
?>