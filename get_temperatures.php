<?php
include "config.php"; // Fichier de config de la connexion à la BDD

// Vérification de la présence du paramètre $n dans l'url, si oui prendre sa valeur, sinon $n = 20
if (isset($_GET["n"])) { 
    $n = (int)$_GET["n"];
} else {
    $n = 20;
}

$rows = $pdo->query("SELECT * FROM meteo ORDER BY time DESC LIMIT $n")->fetchAll(PDO::FETCH_ASSOC); //Possibilité d'injection SQL, à secure (valeur $n en clair sans sanitarisation) 
                                                                        // Récupère les résultats, associe les numéros de colonnes à leurs noms pour plus de clarté

foreach ($rows as $row) {
    echo "ID : {$row['id']} | Température : {$row['temperature']}°C | RSSI : {$row['rssi']} | Date : {$row['time']}<br>";
}
?>