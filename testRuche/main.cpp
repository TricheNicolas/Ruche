#include <iostream>
#include <string>
#include <Windows.h>
#include "serialib.h"

int main()
{

    serialib serialGPS;
    char trameGPS[200];


    // Configuration de la communication série
    const std::string portCOM("COM3");
    serialGPS.Open("COM3", 9600);

    // Attente de 2 secondes pour laisser le temps au module de s'initialiser
    Sleep(2000);
 
    // Envoi d'une commande AT pour récupérer le numéro IMEI
    serialGPS.WriteString("AT+CGSN\r\n");

    // Lecture de la réponse du module
    std::cout << serialGPS.ReadString(trameGPS,'\n',200,5000);

    // Affichage de la réponse
    //std::cout << "Numéro IMEI : " << response << std::endl;

    // Envoi d'une commande AT pour activer le mode GPS
    serialGPS.WriteString("AT+CGNSPWR=1\r\n");

    // Attente de 1 seconde pour laisser le temps au module de s'activer
    Sleep(1000);

    // Envoi d'une commande AT pour récupérer les données GPS
    serialGPS.WriteString("AT+CGNSINF\r\n");

    // Lecture de la réponse du module
    std::cout << serialGPS.ReadString(trameGPS, '\n', 200, 5000);

    // Affichage de la réponse
    //std::cout << "Données GPS : " << response << std::endl;
   
    // Fermeture de la communication série

    serialGPS.Close();

    return 0;
}