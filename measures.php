<?php
include "config.php";

$temperature = isset($_GET['temperature']) ? (float)$_GET['temperature'] : NULL;
$rssi = isset($_GET['rssi']) ? (int)$_GET['rssi'] : NULL;

// Check si l'une des deux valeurs est null | les deux pipes siginifient OU | === pour vérifier la valeur ET le type
if ($temperature === null || $rssi === null) {
    http_response_code(400);
    die("Paramètres manquants");
}

$pdo->query("INSERT INTO meteo (temperature, rssi) VALUES ($temperature, $rssi)");

echo "Valeurs enregistrées";
?>