#include "Gps.h"
#include "Accelerometre.h"
#include <fstream>

int main()
{

    /*********************************************Test Lecture auto port*********************************************/
    /*
    FILE* myPipe = NULL;
    char buffer[1000];
    myPipe = popen("cd | ls /dev/ttyUSB*", "r");
    if (myPipe == NULL) {
        //errorstuff
    }
    while (fgets(buffer, 1000, myPipe) != NULL) {
        (void)printf("\n\nvotre usb est : \n\n%s \n", buffer);
    }
    std::string portUsb = buffer;
    pclose(myPipe);
    */
    /*********************************************Declaration des variables *********************************************/

    const int TAILLE = 512;
    char trame[TAILLE];
    const unsigned int TEMPSREPONSE = 6000;
    const int BAUD = 9600;
    const char I2C_BUS[] = "/dev/i2c-1";

    std::string portCOM("/dev/ttyUSB0");
    std::string latitude, longitude;

    Gps gps;
    Accelerometre accelerometre;
    serialib serial;

    bool boucleMenu = true, verifTrameGPS = false, chute = false;

    int fd, I2C_ADDR = 0x6A/*6A car résultat du i2cdetect*/, registre, commande, valX = 0, valY = 0, valZ = 0;

    /*********************************************Ouverture voie serie *********************************************/
    try
    {
        gps.connexion(portCOM, BAUD);

        /*********************************************commande AT*********************************************/
        memset(trame, 0, sizeof(trame));

        gps.ecriture("ATE1\r\n");

        for (int i = 0; i < 3; i++)
        {
            gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " \t\t" << trame << std::endl;
        }

        /*********************************************commande pour choisir trame rmc*********************************************/
        memset(trame, 0, sizeof(trame));

        gps.ecriture("AT$GPSNMUN=0,0,0,0,0,1,0\r\n");// <enable>,<GGA>,<GLL>,<GSA>,<GSV>,<RMC>,<VTG > donc on veut que les RMC

        std::cout << "\n\t\t################## Ecriture AT$GPSNMUN=0,0,0,0,0,1,0 OK##################\n\n";

        for (int i = 0; i < 3; i++)
        {
            gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " \t\t" << trame << std::endl;
        }

        /*********************************************commande activer envoi trame*********************************************/

        // Nettoyer contenu de trame
        memset(trame, 0, sizeof(trame));

        // Envoi d'une commande AT pour récupérer les données GPS
        gps.ecriture("AT$GPSNMUN=1\r\n");
        std::cout << "\n\t\t################## Ecriture AT$GPSNMUN=1 OK##################\n\n";

        for (int i = 0; i < 5; i++)
        {
            gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " \t\t" << trame << std::endl;
        }
        std::string trameRecup = trame;

        /*********************************************commande arret envoi trame*********************************************/
        memset(trame, 0, sizeof(trame));

        gps.ecriture("AT$GPSNMUN=0\r\n");
        std::cout << "\n\t\t################## Ecriture AT$GPSNMUN=0 OK##################\n\n";
        for (int i = 0; i < 4; i++)
        {
            gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " \t\t" << trame << std::endl;
        }

        /*********************************************Gestion Trame*********************************************/

        gps.trouverLatLongi(trameRecup, latitude, longitude);

        float latConv, longConv;

        latConv = gps.LatGPS(latitude);
        longConv = gps.LongGPS(longitude);

        std::cout << "Trame final a envoyer : \n\t Latitude :  " << latConv << " \n\t Longitude : " << longConv << std::endl;

        gps.mutateurLatitude(latConv);
        gps.mutateurLongitude(longConv);



        memset(trame, 0, sizeof(trame));
        // gps.~Gps();
    }

    catch (const std::exception& e)
    {
        // Si pb affichage du message
        std::cerr << "\nErreur en rapport avec le GPS !\n\n";
        std::cerr << e.what();
    }

    try
    {
        /*********************************************Partie I2C*********************************************/

        /*********************************************Récupération données*********************************************/

        accelerometre.connexion(fd, I2C_BUS);// ouverture du bus I2C

        accelerometre.selectionAdresse(fd, I2C_ADDR);// selection de l'adresse I2C

        accelerometre.ecriture(fd, registre = 0x10, commande = 0x5F);// Activer l'accéléromètre

        accelerometre.ecriture(fd, registre = 0x0E, commande = 0x00);// Si on veut modifier la plage d'échelle 
        std::cout << "configuration de la plage d'echelle ok\n";

        valX = accelerometre.lecture(fd, registre = 0x28);//registre correspondant à l'axe de X
        std::cout << "lecture X ok\n\tX = " << valX << "\n";

        valY = accelerometre.lecture(fd, registre = 0x2A);//registre correspondant à l'axe de Y
        std::cout << "lecture Y ok\n\tY = " << valY << "\n";

        valZ = accelerometre.lecture(fd, registre = 0x2C);//registre correspondant à l'axe de Z
        std::cout << "lecture Z ok\n\tZ = " << valZ << "\n";


        /*********************************************Lecture Fichier.txt*********************************************/
        
        std::string valXAncien, valYAncien, valZAncien;/*
        std::ifstream monFichierLec("fichierAxe.txt");
        if (monFichierLec.is_open()) {
            std::cout << "Lecture dans un fichier élément par élément" << std::endl;
            // premiere lecture dans le fichier (obligatoire) et affectation aux variables
            monFichierLec >> valXAncien >> valYAncien >> valZAncien;
            // tant qu'on a pas atteint la fin du fichier on réalise un affichage puis on boucle
            // il faut bien respecter l'ordre des deux lignes ci-dessous
            std::cout << valXAncien << "->" << valYAncien << "->" << valZAncien << "\n";
            monFichierLec.close();
        }
        else std::cout << "Pb ouverture fichier..." << std::endl;
        */
        /*********************************************Comparaison ancienne et valeur lue *********************************************/

        if (std::stof(valXAncien) > valX + 50 || std::stof(valXAncien) - 50 > valX) chute = true;
        else if (std::stof(valYAncien) > valY + 50 || std::stof(valYAncien) - 50 > valY) chute = true;
        else if (std::stof(valZAncien) > valZ + 50 || std::stof(valZAncien) - 50 > valZ) chute = true;
        else chute = false;
        accelerometre.mutateurChute(chute);//on modifie la valeur pour la renvoyer dans le toString plus tard

        /*********************************************Écriture Fichier.txt*********************************************/
        /*
        std::ofstream monFichier("fichierAxe.txt", std::ios_base::trunc);

        // Le fichier a-t-il été créé ou écrasé ?
        if (monFichier.is_open()) {
            // Ecriture dans le fichier -> toutes les variables sont transformées en texte
            monFichier << valX << " " << valY << " "<<valZ;
            // Fermeture du fichier
            monFichier.close();
            std::cout << "Fichier créé, données enregistrées...\n";
        }
        // sinon on affiche un message à l'utilisateur
        else std::cout << "Probleme de création ecriture du fichier\n";
        */
    }
    catch (const std::exception& e)
    {
        std::cerr << "\t\tErreur sur I2C !!\n";
        std::cerr << "\t\t" << e.what();
    }

    close(fd);// on ferme le bus I2C

    try
    {

        /*********************************************commande pour activer SIM*********************************************/
        std::string verifPIN, verifTest; // test AT+CPIN

        gps.ecriture("AT+CPIN?\r\n");
        for (int i = 0; i < 4; i++)
        {
            gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " "  << "\t\t" << trame << std::endl;
        }

        /*********************************************Passer en mode Texte *********************************************/
        memset(trame, 0, sizeof(trame));

        gps.ecriture("AT+CMGF=1\r\n");
        for (int i = 0; i < 4; i++)
        {
            gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " \t\t" << trame << std::endl;
        }

        /*********************************************commande pour envoyer un message *********************************************/
        memset(trame, 0, sizeof(trame));
        std::string message = "AT+CMGS=\"+33771829830\"\r\n\t" + gps.toString() + accelerometre.toString() + "\x1A";

        gps.ecriture(message.c_str());

        for (int i = 0; i < 10; i++)
        {
            gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " \t\t" << trame << std::endl;
        }

        memset(trame, 0, sizeof(trame));
        gps.~Gps();
    }

    catch (const std::exception& e)
    {
        // Si pb affichage du message
        std::cerr << "\n Erreur Envoie message !\n\n";
        std::cerr << e.what();
        gps.~Gps();

    }// VIDER LE BUFFER CAR C EST CA QUI FOUT LA MERDE GL HF POUR TROUVER CETTE ERREUR A LA CON 

    return 0;

}