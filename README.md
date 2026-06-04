Projet de première année visant à lire les données météo d'une station distante, avec un Feather M0 pour la lecture LoRa, un ESP32 pour récupérer les informations en série et les transmettre en http au serveur web, et un raspberry servant de serveur LAMP

Je me suis focalisé sur cette dernière partie, la mise en place de la BDD, ainsi que le site web en PHP.

Utilisation :

url/get_temperatures.php?n=X permet d'afficher X dernières valeurs enregistrées dans la base, si pas d'option dans l'url, les 20 dernières seront affichées

url/measures?temperature=Y&rssi=Z où Y est la température et Z le rssi, si l'une des deux valeurs est nulle l'enregistrement dans la BDD est annulé
