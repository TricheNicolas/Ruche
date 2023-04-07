#pragma once
#ifndef _Accelerometre_
#define _Accelerometre_

#include <iostream>
#include "smbus.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

class Accelerometre
{
public:

	/** Constructeur de la classe
	* @param 
	* @return pas de retour car constructeur
	*/
	Accelerometre();

	/** permet d'ouvrir le bus i2c
	* @param const int& fd file descriptor représente le descripteur de fichier pour le bus I2C
	* @param const char I2C_BUS pour le numéro du bus
	* @return bool true si pas d'erreur sinon false
	*/
	void ouvrirBus(int& fd, const char I2C_BUS[]);

	/** permet de selectionner l'adresse du bus i2c
	* @param const int& fd file descriptor représente le descripteur de fichier pour le bus I2C
	* @param const char I2C_BUS adresse du bus
	* @return bool true si pas d'erreur sinon false
	*/
	void selectionAdresse(const int& fd, const int& I2C_ADDR);

	/** permet d'écrire sur le bus i2c
	* @param const int& fd file descriptor représente le descripteur de fichier pour le bus I2C
	* @param const int& registre correspond au registre qu'on veut paramétrer
	* @return bool true si pas d'erreur sinon false
	*/
	void ecriture(const int& fd, const int& registre, const int& commande);

	/** permet de lire les données du bus i2c
	* @param const int& fd file descriptor représente le descripteur de fichier pour le bus I2C
	* @param const int& registre correspond au registre qu'on veut paramétrer
	* @return int correspond à la valeur lue
	*/
	int lecture(const int& fd, const int& registre);

	/** destructeur de la classe
	* @param pas de parametre
	* @return ne retourne rien
	*/
	~Accelerometre();

private:

};


#endif // !_Accelerometre_
