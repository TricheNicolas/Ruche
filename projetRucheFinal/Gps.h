#pragma once
#ifndef _GPS_
#define _GPS_

#include <vector>
#include "serialib.h"
#include <sstream>
#include "Capteur.h"

class Gps {

public:

    /** Constructeur de la classe
    * @param const string& pour le numéro du port
    * @param const int& vitesse en baud
    * @return pas de retour car constructeur
    */
    Gps();

    /** Permet la connexion au port série
    * @param Pas de paramètres car on récupère les données au préalable avec le constructeur
    * @return retourne un entier qui précise le niveau d'erreur
    */
    virtual void connexion(const std::string& port, const int& baud);

    /** Permet d'écrire une commande AT
    * @param const char qui contient la commande AT
    * @return retourne un entier qui précise le niveau d'erreur
    */
    virtual void ecriture(const char commandeAT[]);

    /** Permet de lire ce que renvoie le module suite à l'ecriture d'une commande AT
    * @param char* qui est la trame sur laquelle on ecrit ce que renvoie le module
    * @param const char& précise jusqu'a quel caractere on doit lire
    * @param const unsigned int& qui correspond à la taille max qu'on peut lire
    * @param const unsigned int& qui correspond jusqu'a combien de temps
    * @return retourne un entier qui précise le niveau d'erreur
    */
    virtual int lecture(char* trame, const char& caractereFinal, const unsigned int& nbMaxOctets, const unsigned int& tempsReponse);

    /** destructeur de la classe
    * @param pas de paramètre
    * @return pas de retour car destructeur
    */
    ~Gps();

    /** Permet d'extraire la latitude et longitude
    * @param const string& qui est la trame entiere
    * @param string& variable dans laquelle on mettra la latitude
    * @param string& variable dans laquelle on mettra la longitude
    * @return retourne un bool selon si la trame est correcte ou non
    */
    bool trouverLatLongi(const std::string& trameGPS, std::string& latitude, std::string& longitude);

    /** Permet de decouper une chaine
   * @param const string& qui est la chaine entiere
   * @param char le caractere avec lequel on doit separer la chaine
   * @return retourne un vector>string> comportant la chaine decoupee
   */
    std::vector<std::string> scinder(const std::string& s, char delimiter);

    /** Permet de transformer la latitude H.M.S en Degres Decimaux
   * @param string latitude en H.M.S
   * @return retourne un float comportant la latitude en Degres Decimaux
   */
    float LatGPS(std::string lati);

    /** Permet de transformer la longitude H.M.S en Degres Decimaux
    * @param string longitude en H.M.S
    * @return retourne un float comportant la longitude en Degres Decimaux
    */
    float LongGPS(std::string longi);

    /** mutateur du float latitude
    * @param float latitude pour modifier la valeur de latitude
    * @return ne retourne rien
    */
    void mutateurLatitude(float latitude);

    /** accesseur du float latitude
    * @param pas de parametre
    * @return float retourne la valeur de latitude
    */
    float acceusseurLatitude();

    /** mutateur du float longitude
    * @param float longtude pour modifier la valeur de longitude
    * @return ne retourne rien
    */
    void mutateurLongitude(float longitude);

    /** accesseur du float longitude
    * @param pas de parametre
    * @return float retourne la valeur de longitude
    */
    float accesseurLongitude();

    /** toString renvoie un message selon état du bool chute
    * @param pas de parametre
    * @return string renvoie un message
    */
    virtual std::string toString();

private:

    serialib serie;
    std::string port;
    int baud;
    float latitude, longitude;

};

#endif // !_GPS_
