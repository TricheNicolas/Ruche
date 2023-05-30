#pragma once
#ifndef _Accelerometre_
#define _Accelerometre_

#include "smbus.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
//#include <i2c/smbus.h>
#include "Capteur.h"

class Accelerometre  {
public:

	/** Constructeur de la classe
	* @param
	* @return pas de retour car constructeur
	*/
	Accelerometre();

	/** permet d'ouvrir le bus i2c
	* @param const int& fd file descriptor repr�sente le descripteur de fichier pour le bus I2C
	* @param const char I2C_BUS pour le num�ro du bus
	* @return bool true si pas d'erreur sinon false
	*/
	virtual void connexion(int& fd, const char I2C_BUS[]);

	/** permet de selectionner l'adresse du bus i2c
	* @param const int& fd file descriptor repr�sente le descripteur de fichier pour le bus I2C
	* @param const char I2C_BUS adresse du bus
	* @return bool true si pas d'erreur sinon false
	*/
	void selectionAdresse(const int& fd, const int& I2C_ADDR);

	/** permet d'�crire sur le bus i2c
	* @param const int& fd file descriptor repr�sente le descripteur de fichier pour le bus I2C
	* @param const int& registre correspond au registre qu'on veut param�trer
	* @return bool true si pas d'erreur sinon false
	*/
	virtual void ecriture(const int& fd, const int& registre, const int& commande);

	/** permet de lire les donn�es du bus i2c
	* @param const int& fd file descriptor repr�sente le descripteur de fichier pour le bus I2C
	* @param const int& registre correspond au registre qu'on veut param�trer
	* @return int correspond � la valeur lue
	*/
	virtual int lecture(const int& fd, const int& registre);

	/** destructeur de la classe
	* @param pas de parametre
	* @return ne retourne rien
	*/
	~Accelerometre();

	/** mutateur du bool chute
	* @param bool chute qui sert � dire si la ruche est tomb�e ou non
	* @return ne retourne rien
	*/
	void mutateurChute(bool chute);

	/** accesseur du bool chute
	* @param pas de parametre
	* @return bool si true alors la ruche est tomb�e sinon false
	*/
	bool accesseurChute();

	/** toString renvoie un message selon �tat du bool chute
	* @param pas de parametre
	* @return string renvoie un message
	*/
	virtual std::string toString();

private:
	bool chute;
};


#endif // !_Accelerometre_
