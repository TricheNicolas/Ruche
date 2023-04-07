
#include "serialib.h"
#include "Gps.h"
#include "smbus.h"
#include "Accelerometre.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>



int complement_a_deux(uint16_t value)
{
    // Conversion en complément à deux
    if (value & 0x8000) {
        value = -(0x10000 - value);
    }

    return (int)value;
}

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
    const unsigned int TEMPSREPONSE = 5000;
    const int BAUD = 9600;

    std::string portCOM("/dev/ttyUSB0");
    Gps gps(portCOM, BAUD);
    bool boucleMenu = true;
    bool verifTrameGPS = false;

    int erreur = 0;

    Accelerometre accelerometre;

    const char I2C_BUS[] = "/dev/i2c-1";

    int fd,I2C_ADDR = 0x6A,registre,commande, valX = 0, valY = 0, valZ = 0 ,valXOld = 0,valYOld = 0, valZOld = 0;

    bool chute = false;
    bool premiereVerif = false;

    /*********************************************Ouverture voie serie *********************************************/
    
    do
    {
        int choix = 0;
        std::cout << "\n#################################### Bienvenue sur le projet ruche quel choix voulez-vous faire ? ####################################\n\n";
        std::cout << "\t- Choix n°1 : Recuperer les donnees du GPS.\n\t- Choix n°2 : Recuperer les donnes de l'accelerometre.\n\t- Choix n°3 : Quitter le programme\n\n\tPour faire votre choix veuillez rentrer uniquement le numero du choix\n\n";
        std::cin >> choix;

        switch (choix)
        {
        case 1 : // cas ou l'utilisateur choisi les donnes GPS
            try
            {
                erreur = gps.connexion();

                /*********************************************commande AT*********************************************/

                erreur = gps.ecriture("ATE1\r\n");
                for (int i = 0; i < 3; i++)
                {
                    erreur = gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
                    if (erreur == 2)
                    {
                        //trame[TAILLE] = '\ttest';

                    }
                    else  std::cout << i << " " << erreur << "\t\t" << trame << std::endl;
                }

                /*
                // Envoi d'une commande AT pour récupérer le numéro IMEI
                erreur = gps.ecriture("AT+CGSN\r\n");

                if (erreur < 0)
                {
                    std::cout << "erreur CGSN\n";
                    return 0;
                }
                std::cout << "\n\t\t################## Ecriture CGSN OK##################\n\n";

                for (int i = 0; i < 5; i++)
                {
                    erreur = gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
                    std::cout << i << " " << erreur << "\t\t" << trame << std::endl;
                }
                */
                /*********************************************commande AT$GPSNMUN*********************************************/

                erreur = gps.ecriture("AT$GPSNMUN=0,0,0,0,0,1,0\r\n");// <enable>,<GGA>,<GLL>,<GSA>,<GSV>,<RMC>,<VTG > donc on veut que les RMC

                std::cout << "\n\t\t################## Ecriture AT$GPSNMUN=0,0,0,0,0,1,0 OK##################\n\n";

                for (int i = 0; i < 3; i++)
                {
                    erreur = gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
                    std::cout << i << " " << erreur << "\t\t" << trame << std::endl;
                }

                /*********************************************commande AT$GPSNMUN = 1*********************************************/

                // Envoi d'une commande AT pour récupérer les données GPS
                erreur = gps.ecriture("AT$GPSNMUN=1\r\n");
                std::cout << "\n\t\t################## Ecriture AT$GPSNMUN=1 OK##################\n\n";

                for (int i = 0; i < 5; i++)
                {
                    erreur = gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
                    std::cout << i << " " << erreur << "\t\t" << trame << std::endl;
                }
                std::string trameRecup = trame;

                /*********************************************commande AT$GPSNMUN = 0*********************************************/

                erreur = gps.ecriture("AT$GPSNMUN=0\r\n");
                std::cout << "\n\t\t################## Ecriture AT$GPSNMUN=0 OK##################\n\n";

                for (int i = 0; i < 4; i++)
                {
                    erreur = gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
                    std::cout << i << " " << erreur << "\t\t" << trame << std::endl;
                }

                /*********************************************Gestion Trame*********************************************/
                std::string latitude, longitude;
                gps.trouverLatLongi(trameRecup, latitude, longitude);

                std::cout << "Trame avant conversion : \n\t Latitude :  " << latitude << " \n\t Longitude : " << longitude << std::endl;

                float latConv, longConv;
                latConv = gps.LatGPS(latitude);
                longConv = gps.LongGPS(longitude);

                std::cout << "Trame final a envoyer : \n\t Latitude :  " << latConv << " \n\t Longitude : " << longConv << std::endl;

                verifTrameGPS = true;

                gps.~Gps();

            }
            
            catch (const std::exception& e)
            {
                // Si pb affichage d'un un message
                std::cerr << "\nErreur en rapport avec le GPS !\n\n";
                std::cerr << e.what();
            }
     
            break;
        case 2 : // cas ou l'utilisateur choisi les donnes de l'accelerometre

            try
            {

            valXOld = valX;
            valYOld = valY;
            valZOld = valZ;

            accelerometre.ouvrirBus(fd, I2C_BUS);

            accelerometre.selectionAdresse(fd, I2C_ADDR);

            accelerometre.ecriture(fd, registre = 0x10, commande = 0x5F);

            accelerometre.ecriture(fd, registre = 0x0E, commande = 0x00);
            std::cout << "configuration de la plage d'echelle ok\n";

            valX = accelerometre.lecture(fd, registre = 0x28);
            std::cout << "lecture X ok\n\tX = " << valX << "\n";

            valY = accelerometre.lecture(fd, registre = 0x2A);
            std::cout << "lecture Y ok\n\tY = " << valY << "\n";

            valZ = accelerometre.lecture(fd, registre = 0x2C);
            std::cout << "lecture Z ok\n\tZ = " << valZ << "\n";

            if (premiereVerif == true)
            {
                if (valXOld > valX + 50 || valXOld - 50 > valX) chute = true;
                else if (valYOld > valY + 50 || valYOld - 50 > valY) chute = true;
                else if (valZOld > valZ + 50 || valZOld - 50 > valZ) chute = true;
                else {
                    std::cout << " toujours debout, toujours vivant\n";
                    chute = false;
                }
                if (chute == true)std::cout << "un invocateur a quitte la partie.\n";
            }
            else premiereVerif = true;
            
            }
            catch (const std::exception& e)
            {
                std::cerr << "\t\tErreur sur I2C !!\n";
                std::cerr << "\t\t" <<e.what();
            }

            close(fd);


            break;
        case 3: // cas ou l'utilisateur souhaite quitter
            boucleMenu = false;
            std::cout << "\tVous avez bien quitte le programme.\n";
            break;
        default:
            std::cout << "Erreur lors de la sasie du choix, veuillez recommencer\n";
        }

    } while (boucleMenu == true);


   
    /*********************************************Partie test envoie MSG *********************************************/
    try
    {
        erreur = gps.connexion();

        /*********************************************commande ATE1 (afficher commande)*********************************************/

        erreur = gps.ecriture("ATE1\r\n");
        for (int i = 0; i < 3; i++)
        {
            erreur = gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " " << erreur << "\t\t" << trame << std::endl;
        }

        /*********************************************commande AT+CGMM(information about the model of the device)*********************************************/
        /*
        erreur = gps.ecriture("AT+CGMM?\r\n");
        for (int i = 0; i < 3; i++)
        {
            erreur = gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " " << erreur << "\t\t" << trame << std::endl;
        }*/

        /*********************************************commande AT+CFUN? (level of functionality)*********************************************/
        /*
        erreur = gps.ecriture("AT+CFUN?\r\n");
        for (int i = 0; i < 3; i++)
        {
            erreur = gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " " << erreur << "\t\t" << trame << std::endl;
        }*/

        /*********************************************commande AT+CPIN? *********************************************/
        std::string verifPIN;
        std::string verifTest;
        serialib serial;

        erreur = gps.ecriture("AT+CPIN?\r\n");
        for (int i = 0; i < 4; i++)
        {
            erreur = gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " " << erreur << "\t\t" << trame << std::endl;
            /*
                char caracRecu;
                serial.ReadChar(&caracRecu);
                if (caracRecu == '+') {
                    verifPIN = trame;
                    std::cout << "coucou je suis ici\n";
                }*/
        }
        /*verifTest = trame;
        std::cout << "test "<< verifPIN<< "\n";
        std::cout << "test2 "<< verifTest<< "\n";
        std::vector<std::string> morceauPIN;
        morceauPIN = gps.split(verifPIN, ' ');
        std::cout << morceauPIN.at(0) <<"\n";*/

        /*********************************************commande AT+CSCS="IRA" *********************************************/

        erreur = gps.ecriture("AT+CSCS=\"IRA\"\r\n");
        for (int i = 0; i < 3; i++)
        {
            erreur = gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " " << erreur << "\t\t" << trame << std::endl;
        }

        /*********************************************commande AT+CMGF? *********************************************/

        erreur = gps.ecriture("AT+CMGF=1\r\n");
        for (int i = 0; i < 3; i++)
        {
            erreur = gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " " << erreur << "\t\t" << trame << std::endl;
        }

        

        /*********************************************commande AT+CMGS="+33xxxxxxxxx *********************************************/

        erreur = gps.ecriture("AT+CMGS=\"+33771829830\",\"13\"\r\n");
        for (int i = 0; i < 3; i++)
        {
            erreur = gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            std::cout << i << " " << erreur << "\t\t" << trame << std::endl;
        }

        gps.~Gps();
    }
    
    catch (const std::exception& e)
    {
        // Si pb affichage d'un un message
        std::cerr << "\n Erreur Envoie message !\n\n";
        std::cerr << e.what();
    }


        /*********************************************commande AT*********************************************/

       



    /*********************************************Partie I2C *********************************************/


    /*
    // Ouvrir le bus I2C
    fd = open(test, O_RDWR);
    if (fd < 0) {
        std::cout <<"Erreur lors de l'ouverture du bus I2C\n";
        exit(1);
    }
    else
    {
        std::cout << "ok " << fd <<" \n";
    }
    */

    // Sélectionner l'adresse I2C de l'accéléromètre
   /* if (ioctl(fd, I2C_SLAVE, I2C_ADDR) < 0) {
        std::cout << "Erreur lors de la sélection de l'adresse I2C de l'accéléromètre\n";
        exit(1);
    }
    else
    {
        std::cout << "ok " << fd << " \n";
    }
    */

    // Activer l'accéléromètre
    /*if (i2c_smbus_write_byte_data(fd, 0x10, 0x5F) < 0) {
        std::cout << "Erreur lors de l'activation de l'accéléromètre\n";
        exit(1);
    }
    else
    {
        std::cout << "ok " << fd << " \n";
    }*/


    /*
    if (i2c_smbus_write_byte_data(fd, 0x0E, 0x00) < 0) {
        std::cout << "Erreur lors de la configuration de la plage d'echelle\n";
        exit(1);
    }
    else
    {
        std::cout << "Plage d'échelle configurée avec succes\n";
    }
    */

    /*
    int erreurY = i2c_smbus_read_word_data(fd, 0x28);
    std::cout << erreurY << "\n";

    int erreurY = i2c_smbus_read_word_data(fd, 0x2A);
    std::cout << erreurY << "\n";

    int erreurZ = i2c_smbus_read_word_data(fd, 0x2C);
    std::cout << erreurZ << "\n";
    */
    /*
    // Convertir les valeurs hexadécimales en valeurs numériques
    int x = complement_a_deux(valX);
    int y = complement_a_deux(valY);
    int z = complement_a_deux(valZ);

    // Afficher les valeurs d'accélération
    std::cout << "Accélération hexa :\n";
    std::cout << "X = " << std::hex << valX << std::dec << "\n";
    std::cout << "Y = " << std::hex << valY << std::dec << "\n";
    std::cout << "Z = " << std::hex << valZ << std::dec << "\n";

    std::cout << "Accélération :\n";
    std::cout << "X = " << x << "\n";
    std::cout << "Y = " << y << "\n";
    std::cout << "Z = " << z << "\n";
    */
    /*
    bool chute = false;

    if (valX > valX + 50 || valX < valX - 50)
    {
        chute = true;
        std::cout << "tombe sur l axe X\n";
    }
    else if (valY > valY + 50 || valY < valY - 50)
    {
        chute = true;
        std::cout << "tombe sur l axe Y\n";
    }
    else if (valZ > valZ + 50 || valZ < valZ - 50)
    {
        chute = true;
        std::cout << "tombe sur l axe Z\n";
    }
    else std::cout << " toujours debout, toujours vivant\n";
    */
    // Fermer le bus I2C
    //close(fd);

    return 0;

}