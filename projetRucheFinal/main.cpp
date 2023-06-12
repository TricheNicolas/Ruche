﻿#include "Gps.h"
#include "Accelerometre.h"
#include <fstream>
#include <thread>
#include <cstdio>

int main()
{

    /*********************************************Lecture auto port*********************************************/
    
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
    portUsb = portUsb.substr(0, 12);
    pclose(myPipe);
    
    /*********************************************Declaration des variables *********************************************/

    const int TAILLE = 512;
    char trame[TAILLE];
    const unsigned int TEMPSREPONSE = 6000;
    const int BAUD = 9600;
    const char I2C_BUS[] = "/dev/i2c-1";
    const int NOMBREREPETITIONS = 4;

    std::string latitude, longitude;
    std::string valXAncien, valYAncien, valZAncien;
    std::string recup;

    Gps gps;
    Accelerometre accelerometre;
    serialib serial;

    bool boucleMenu = true, verifTrameGPS = false, chute = false;
    bool fin = false;

    int fd, I2C_ADDR = 0x6A/*6A car résultat du i2cdetect*/, registre, commande, valX = 0, valY = 0, valZ = 0, i=0;

    enum class etape {
        AttenteOuvertureVoieSerie,RecuperationDonnesGPS,VerificationDonneesGPS,AttentionConnexionI2C,RecuperationDonneesI2C,VerificationEtatRuche,VerificationCarteSIM,EnvoyerMessage
    };

    std::ifstream monFichierLec("/home/pi/fichierASupprimer.txt");
    if (monFichierLec.is_open()) {
        monFichierLec.close();
        std::cout << "La voie serie est en cours d'execution\n\n";
        sleep(20);
    }
    else std::cout << "Aucune tâche en cours d'execution\n";

    std::ofstream monFichier("/home/pi/fichierASupprimer.txt", std::ios_base::trunc);// Le fichier a été créé ou écrasé
    if (monFichier.is_open()) {
        monFichier.close();// Fermeture du fichier
        std::cout << "Fichier créé...\n";
    }
    else std::cout << "Probleme de création ecriture du fichier\n";// sinon on affiche un message à l'utilisateur 

    gps.~Gps();
    etape etapeActive = etape::AttenteOuvertureVoieSerie;
    do
    {       
          switch (etapeActive)
        {
        case etape::AttenteOuvertureVoieSerie:
            /*********************************************Ouverture voie serie *********************************************/
            try
            {
                gps.connexion(portUsb, BAUD);      

                etapeActive = etape::RecuperationDonnesGPS;
            }
            catch (const std::exception& e)
            {
                std::cerr << "\nErreur AttenteConnexionSerie !\n\n" << e.what(); // Si pb affichage du message
            }
            break;
        case etape::RecuperationDonnesGPS:
            try
            { 
                gps.recupererDonneesGPS(trame, TAILLE, TEMPSREPONSE, recup);
                etapeActive = etape::VerificationDonneesGPS;
            }
            catch (const std::exception& e)
            {
                std::cerr << "\nErreur RecuperationDonnesGPS !\n\n" << e.what(); // Si pb affichage du message
                i++;
                if (i == NOMBREREPETITIONS)
                {
                    i = 0;
                    fin = true;
                }
            }
            break;
        case etape::VerificationDonneesGPS:
            try
            {
                /*********************************************Gestion Trame*********************************************/
                float latConv, longConv;

                gps.trouverLatLongi(recup, latitude, longitude);
                latConv = gps.LatGPS(latitude);
                longConv = gps.LongGPS(longitude);

                std::cout << "Trame final a envoyer : \n\t Latitude :  " << latConv << " \n\t Longitude : " << longConv << std::endl;

                gps.mutateurLatitude(latConv);
                gps.mutateurLongitude(longConv);
                etapeActive = etape::AttentionConnexionI2C;
            }
            catch (const std::exception& e)
            {
                std::cerr << "\nErreur VerificationDonneesGPS !\n\n" << e.what(); // Si pb affichage du message
                etapeActive = etape::RecuperationDonnesGPS;
            }
            break;
        case etape::AttentionConnexionI2C:
            try
            {
                /*********************************************Partie I2C*********************************************/
                /*********************************************Récupération données*********************************************/
                accelerometre.connexion(fd, I2C_BUS);// ouverture du bus I2C
                accelerometre.selectionAdresse(fd, I2C_ADDR);// selection de l'adresse I2C
                etapeActive = etape::RecuperationDonneesI2C;
            }
            catch (const std::exception& e)
            {               
                std::cerr << "\nErreur AttentionConnexionI2C !\n\n" << e.what(); // Si pb affichage du message
                i++;
                if (i == NOMBREREPETITIONS)
                {
                    i = 0;
                    fin = true;
                }
            }
            break;
        case etape::RecuperationDonneesI2C:
            try
            {
                accelerometre.ecriture(fd, registre = 0x10, commande = 0x5F);// Activer l'accéléromètre
                //accelerometre.ecriture(fd, registre = 0x0E, commande = 0x00);// Si on veut modifier la plage d'échelle 
                //std::cout << "configuration de la plage d'echelle ok\n";
                valX = accelerometre.lecture(fd, registre = 0x28);//registre correspondant à l'axe de X
                std::cout << "lecture X ok\n\tX = " << valX << "\n";
                valY = accelerometre.lecture(fd, registre = 0x2A);//registre correspondant à l'axe de Y
                std::cout << "lecture Y ok\n\tY = " << valY << "\n";
                valZ = accelerometre.lecture(fd, registre = 0x2C);//registre correspondant à l'axe de Z
                std::cout << "lecture Z ok\n\tZ = " << valZ << "\n";
                etapeActive = etape::VerificationEtatRuche;
            }
            catch (const std::exception& e)
            {
                std::cerr << "\nErreur RecuperationDonneesI2C !\n\n" << e.what(); // Si pb affichage du message
                i++;
                if (i == NOMBREREPETITIONS)
                {
                    i = 0;
                    fin = true;
                }
            }
            break;
        case etape::VerificationEtatRuche:
            try
            {
                /*********************************************Lecture Fichier.txt*********************************************/
                std::ifstream monFichierLec("fichierAxe.txt");
                if (monFichierLec.is_open()) {
                    std::cout << "Lecture dans un fichier élément par élément" << std::endl; // premiere lecture dans le fichier (obligatoire) et affectation aux variables           
                    monFichierLec >> valXAncien >> valYAncien >> valZAncien;
                    std::cout << valXAncien << "->" << valYAncien << "->" << valZAncien << "\n";
                    monFichierLec.close();
                }
                else std::cout << "Pb ouverture fichier..." << std::endl;
                
                            /*********************************************Comparaison ancienne et valeur lue *********************************************/
                if (std::stoi(valXAncien) > valX + 50 || std::stoi(valXAncien) < valX - 50) {
                    chute = true; std::cout << "X decale\n";
                    accelerometre.mutateurChute(chute);//on modifie la valeur pour la renvoyer dans le toString plus tard
                }
                else if (std::stoi(valYAncien) > valY + 50 || std::stoi(valYAncien) < valY - 50) {
                    chute = true; std::cout << "Y decale\n";
                    accelerometre.mutateurChute(chute);//on modifie la valeur pour la renvoyer dans le toString plus tard
                }
                else if (std::stoi(valZAncien) > valZ + 50 || std::stoi(valZAncien) < valZ - 50) {
                    chute = true; std::cout << "Z decale\n";
                    accelerometre.mutateurChute(chute);//on modifie la valeur pour la renvoyer dans le toString plus tard
                }
                else {
                    chute = false;
                }
                /*********************************************Écriture Fichier.txt*********************************************/
                std::ofstream monFichier("fichierAxe.txt", std::ios_base::trunc);// Le fichier a été créé ou écrasé
                if (monFichier.is_open()) {                    
                    monFichier << valX << " " << valY << " " << valZ;// Ecriture dans le fichier -> toutes les variables sont transformées en texte                    
                    monFichier.close();// Fermeture du fichier
                    std::cout << "Fichier créé, données enregistrées...\n";
                }                
                else std::cout << "Probleme de création ecriture du fichier\n";// sinon on affiche un message à l'utilisateur
                etapeActive = etape::VerificationCarteSIM;
                close(fd);
            }
            catch (const std::exception& e)
            {
                std::cerr << "\nErreur VerificationEtatRuche !\n\n" << e.what(); // Si pb affichage du message
                i++;
                if (i == NOMBREREPETITIONS)
                {
                    i = 0;
                    fin = true;
                }
            }
            break;
        case etape::VerificationCarteSIM:
            try
            {
                /*********************************************commande pour activer SIM*********************************************/
                memset(trame, 0, sizeof(trame));
                std::vector<std::string> vecTrame;
                gps.ecriture("AT+CPIN?\r\n");
                sleep(1);
                for (int i = 0; i < 5; i++)
                {
                    gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
                    //std::cout << i << " " << "\t\t" << trame << std::endl;
                    vecTrame.push_back(trame);
                }
                for (int i = 0; i < vecTrame.size(); i++)
                {
                    if (vecTrame.at(i) != "OK\r\n" && vecTrame.at(i) != "AT+CPIN?\r\n" && vecTrame.at(i) != "\r\n" && vecTrame.at(i) != "+CPIN: READY\r\n")
                    {
                        //std::cout <<"AAAAAAAAAAAAAAAAAAA"<< vecTrame.at(i);
                        recup = vecTrame.at(i);
                        i = vecTrame.size();
                        gps.ecriture("AT+CPIN=8428\r\n");
                        sleep(1);
                        for (int i = 0; i < 4; i++)
                        {
                            gps.lecture(trame, '\n', TAILLE, TEMPSREPONSE);
                            //std::cout << i << " " << "\t\t" << trame << std::endl;
                        }
                        gps.~Gps();
                        //etapeActive = etape::AttenteOuvertureVoieSerie;
                        fin = true;
                        //break;
                    }
                    else {
                        //std::cout<< "BBBBBBBBBBBBB" << vecTrame.at(i);
                        etapeActive = etape::EnvoyerMessage;
                    }
                }
            }
            catch (const std::exception& e)
            {                
                std::cerr << "\nErreur en rapport avec le GPS !\n\n" << e.what();// Si probleme affichage du message
                i++;
                if (i == NOMBREREPETITIONS)
                {
                    i = 0;
                    fin = true;
                }
            }
            break;
        case etape::EnvoyerMessage:
            /*********************************************Passer en mode Texte *********************************************/
            try
            {             
                std::cout << " AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n";
                gps.envoyerMessage(trame, TAILLE, TEMPSREPONSE);
                std::cout << " ptn pitie\n";
                gps.~Gps();
                fin = true;
            }
            catch (const std::exception& e)
            {
                std::cerr << "\nErreur EnvoyerMessage !\n\n" << e.what();// Si probleme affichage du message
                i++;
                if (i == NOMBREREPETITIONS)
                {
                    i = 0;
                    fin = true;
                }
            }
            break;
        default:
            std::cerr << "\n\t cho le spawnkill\n";
            break;
        }
    } while (fin == false);

    const char* ficTmp = "/home/pi/fichierASupprimer.txt";
    int result = std::remove(ficTmp);

    if (result == 0)std::cout << "Fichier supprime avec succès\n";
    else std::cout << "Impossible de supprimer le fichier\n";

    return 0;

}