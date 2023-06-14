#include "Gps.h"
#include "Accelerometre.h"

Gps::Gps()
{
}

void Gps::connexion(const std::string& port, const int& baud)
{
    int erreur = serie.Open(port.c_str(), baud);//Open �tant une fonction serialib
    switch (erreur)
    {
    case 254: throw std::runtime_error("Appareil non reconnu, verifiez branchement ou bien le nom du port !\n");//throw permet de relever l'erreur
        break;
    case 252: throw std::runtime_error("Erreur baud\n");
        break;
    default:
        //throw std::runtime_error("OSEKOUR\n");
        break;
    }
}

void Gps::ecriture(const char commandeAT[])
{
    int erreur = serie.WriteString(commandeAT);//WriteString �tant une fonction serialib
    std::string commande = commandeAT;
    if (erreur < 0)
    {
        throw std::runtime_error("Erreur d'ecriture sur la commande" + commande + "\n");
    }
}

int Gps::lecture(char* trame, const char& caractereFinal, const unsigned int& nbMaxOctets, const unsigned int& tempsReponse)
{
    int erreur = serie.ReadString(trame, caractereFinal, nbMaxOctets, tempsReponse);//ReadString �tant une fonction serialib
    switch (erreur)
    {
    case 0:
        throw std::runtime_error("timeout atteint\n");
        break;
    case -1:
        throw std::runtime_error("erreur pendant le timeout\n");
        break;
    case -2:
        throw std::runtime_error("erreur pendant l'ecriture des octets\n");
        break;
    case -3:
        throw std::runtime_error("nbMaxOctets est atteint\n");
        break;
    default:
        break;
    }
    if (erreur < 0)
    {
        throw std::runtime_error("Erreur de lecture\n");
    }
}

bool Gps::quitter(int& i,const int& NOMBREREPETITIONS,bool& fin)
{
    i++;
    if (i == NOMBREREPETITIONS)
    {
        i = 0;
        fin = true;
        return true;
    }
    return false;
}

bool Gps::verificationReceptionMessage(char trame[], const int& TAILLE, const unsigned int& TEMPSREPONSE, std::string& numero, std::string& recup, std::vector<std::string>& numeroTelephone)
{
    memset(trame, 0, sizeof(trame));//permet de vider la trame
    ecriture("ATE1\r\n");//affiche les commandes AT qu'on envoie
    for (int i = 0; i < 3; i++)
    {
        lecture(trame, '\n', TAILLE, TEMPSREPONSE);
    }
    std::vector<std::string> vecTrame;
    memset(trame, 0, sizeof(trame));
    ecriture("AT+CMGL=\"REC UNREAD\"\r\n");//permet la lecture des messages non lus
    sleep(1);
    for (int i = 0; i < 3; i++)
    {
        lecture(trame, '\n', TAILLE, TEMPSREPONSE);
        std::cout << i << " \t\t" << trame << std::endl;
        vecTrame.push_back(trame);
    }
    for (int i = 0; i < vecTrame.size(); i++)
    {
        if (vecTrame.at(i) != "\r\n" && vecTrame.at(i) != "OK\r\n" && vecTrame.at(i) != "AT+CMGL=\"REC UNREAD\"\r\n")//cela permet de voir si on a re�u un message ou non
        {
            recup = vecTrame.at(i);
            i = vecTrame.size();
            std::vector<std::string> recupNumero;
            recupNumero = scinder(recup, '\"');
            numero = recupNumero.at(3);
            lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            recup = trame;
            lecture(trame, '\n', TAILLE, TEMPSREPONSE);
            int taille = numeroTelephone.size();
            for (int i = 0; i < taille; i++)
            {
                if (numeroTelephone.at(i) == numero)return true;//on compare avec les num�ros d�j� enregistr�s
                else return false;
            }
        }
    }

    memset(trame, 0, sizeof(trame));
    ecriture("AT+CMGD=4\r\n");//on supprime tous les messages stock�s par le module car on a une limite de 30
    sleep(1);
    for (int i = 0; i < 2; i++)
    {
        lecture(trame, '\n', TAILLE, TEMPSREPONSE);
    }

}

void Gps::recupererDonneesGPS(char trame[], const int& TAILLE, const unsigned int& TEMPSREPONSE, std::string& recup)
{
    /*********************************************commande AT*********************************************/
    memset(trame, 0, sizeof(trame));
    ecriture("ATE1\r\n");

    for (int i = 0; i < 3; i++)
    {
        lecture(trame, '\n', TAILLE, TEMPSREPONSE);
    }
    /*********************************************commande pour choisir trame rmc*********************************************/
    memset(trame, 0, sizeof(trame));

    ecriture("AT$GPSNMUN=0,0,0,0,0,1,0\r\n");// <enable>,<GGA>,<GLL>,<GSA>,<GSV>,<RMC>,<VTG > donc on veut que les RMC
    sleep(1);
    for (int i = 0; i < 3; i++)
    {
        lecture(trame, '\n', TAILLE, TEMPSREPONSE);
    }
    /*********************************************commande activer envoi trame*********************************************/

    // Nettoyer contenu de trame
    memset(trame, 0, sizeof(trame));
    // Envoi d'une commande AT pour r�cup�rer les donn�es GPS
    ecriture("AT$GPSNMUN=1\r\n");
    std::vector<std::string> vecTrame;
    sleep(1);
    for (int i = 0; i < 10; i++)
    {
        lecture(trame, '\n', TAILLE, TEMPSREPONSE);
        vecTrame.push_back(trame);//on met le contenu de la trame dans un vecteur pour pouvoir la traiter par la suite
    }
    for (int i = 0; i < vecTrame.size(); i++)
    {
        if (vecTrame.at(i) != "OK\r\n" && vecTrame.at(i) != "AT$GPSNMUN=1\r\n" && vecTrame.at(i) != "\r\n")//on recupere la trame re�ue
        {
            recup = vecTrame.at(i);
            i = vecTrame.size();
        }
    }
    /*********************************************commande arret envoi trame*********************************************/
    memset(trame, 0, sizeof(trame));
    ecriture("AT$GPSNMUN=0\r\n");//on coupe l'envoi des trames GPS
    for (int i = 0; i < 2; i++)lecture(trame, '\n', TAILLE, TEMPSREPONSE);
}

void Gps::envoyerMessage(char trame[], const int& TAILLE, const unsigned int& TEMPSREPONSE, Accelerometre accelerometre,std::vector<std::string> repertoire, bool envoieMessageErreur)
{
    memset(trame, 0, sizeof(trame));
    ecriture("AT+CMGF=1\r\n");// on passe en mode texte car =2 correspond au mode hexa
    sleep(1);
    for (int i = 0; i < 1; i++)
    {
        lecture(trame, '\n', TAILLE, TEMPSREPONSE);
    }

    /*********************************************commande pour envoyer un message *********************************************/
    memset(trame, 0, sizeof(trame));
    std::string message;
    int taille = repertoire.size();
    for (int i = 0; i < taille; i++)
    {   
        if (envoieMessageErreur==true)
        {
            message = "AT+CMGS=\"" + repertoire.at(i) + "\"\r\n\tErreur lors de l envoi des donnees\x1A";// contenu du message � envoyer aux num�ros r�pertori�s
        }
        else
        {
            message = "AT+CMGS=\"" + repertoire.at(i) + "\"\r\n\t" + toString() + accelerometre.toString() + "\x1A";// contenu du message � envoyer aux num�ros r�pertori�s
        }
    ecriture(message.c_str());
    sleep(1);
    for (int i = 0; i < 1; i++)
    {
        lecture(trame, '\n', TAILLE, TEMPSREPONSE);
        //std::cout << i << " \t\t" << trame << std::endl;
    }
    memset(trame, 0, sizeof(trame)); 
    sleep(7);
    }
}

std::vector<std::string> Gps::scinder(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

bool Gps::trouverLatLongi(const std::string& trameGPS, std::string& latitude, std::string& longitude)
{
    std::vector<std::string> morceauTrame;
    morceauTrame = scinder(trameGPS, ',');

    if ((morceauTrame.at(0) == "$GPSNMUN: $GPRMC") && (morceauTrame.at(2) == "A")) { // r�cuperer les champs 1,3,4,6
        if (morceauTrame.at(4) == "S") latitude = "-" + morceauTrame.at(3);
        else if (morceauTrame.at(6) == "O") longitude = "-" + morceauTrame.at(5);
        else {
            latitude = morceauTrame.at(3); //pour avoir un entier, prend moins de places qu'un float 
            longitude = morceauTrame.at(5); //pour avoir un entier, prend moins de places qu'un float 
            return true;
        }
    }
    else throw std::runtime_error("Trame non valide, veuillez verifier sa position\n");
    return false;
}

float Gps::LatGPS(std::string lati)
{
    float deg, minute, val;
    int taille = 0;

    taille = lati.size() - 2;
    deg = stof(lati.substr(0, 2));
    minute = stof(lati.substr(2, taille));

    val = deg + (minute / 60);

    return val;
}

float Gps::LongGPS(std::string longi)
{

    float deg, minute, val;
    int taille = 0;

    taille = longi.size() - 3;
    deg = stof(longi.substr(0, 3));
    minute = stof(longi.substr(3, taille));

    val = deg + (minute / 60);

    return val;
}

void Gps::mutateurLatitude(float latitude) {
    this->latitude = latitude;
}

float Gps::acceusseurLatitude() {
    return latitude;
}

void Gps::mutateurLongitude(float longitude) {
    this->longitude = longitude;
}

float Gps::accesseurLongitude() {
    return this->longitude;
}

std::string Gps::toString()
{
    return "Voici le rapport quotidien de la ruche\nLatitude : " + std::to_string(this->latitude) + "\nLongitude : " + std::to_string(this->longitude) + "\nEtat ruche : ";
}

Gps::~Gps()
{
    serie.Close();
}

