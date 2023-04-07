#include "Gps.h"

Gps::Gps(const std::string& port, const int& baud)
{
    this->port = port;
    this->baud = baud;
}

int Gps::connexion()
{
    int erreur = serial.Open(port.c_str(), baud);
    switch (erreur)
    {
    case 1: return erreur;
        break;
    case 254: throw std::runtime_error("Appareil non reconnu, verifiez branchement ou bien le nom du port !\n");
        break;
    case 252: throw std::runtime_error("Erreur baud\n");
        break;
    default:
        throw std::runtime_error("OSEKOUR\n");
        break;
    }
    return erreur;
}

int Gps::ecriture(const char commandeAT[])
{
    int erreur = serial.WriteString(commandeAT);
    std::string commande = commandeAT; 
    if (erreur < 0)
    {
        throw std::runtime_error("Erreur d'ecriture sur la commande" + commande + "\n");
    }
    return erreur;
}

int Gps::lecture(char* trame, const char& caractereFinal, const unsigned int& nbMaxOctets, const unsigned int& tempsReponse)
{
     int erreur = serial.ReadString(trame, caractereFinal, nbMaxOctets, tempsReponse);
    switch (erreur)
    {
    case 0 : 
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
    return erreur;
}

Gps::~Gps()
{
    ecriture("AT+CGNSPWR = 0\r\n");
    serial.Close();
}

std::vector<std::string> Gps::split(const std::string& s, char delimiter)
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
    morceauTrame = split(trameGPS, ',');

    if ((morceauTrame.at(0) == "$GPSNMUN: $GPRMC") && (morceauTrame.at(2) == "A")) { // récuperer les champs 1,3,4,6
        if (morceauTrame.at(4)== "S") latitude = "-" + morceauTrame.at(3);
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



