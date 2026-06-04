<?php
include "config.php"; // Fichier de config de la connexion à la BDD

// Vérification de la présence du paramètre $n dans l'url, si oui prendre sa valeur, sinon $n = 20
$n = isset($_GET["n"]) ? (int)$_GET["n"] : 20;

$stmt = $pdo->prepare("SELECT * FROM meteo ORDER BY time DESC LIMIT :n");
$stmt->bindValue(':n', $n, PDO::PARAM_INT);
$stmt->execute();
$rows = $stmt->fetchAll(PDO::FETCH_ASSOC);                                                                      // Récupère les résultats, associe les numéros de colonnes à leurs noms pour plus de clarté

foreach ($rows as $row) {
    echo "ID : {$row['id']} | Température : {$row['temperature']}°C | RSSI : {$row['rssi']} | Date : {$row['time']}<br>";
}
?>