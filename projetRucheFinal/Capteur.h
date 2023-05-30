#pragma once
#ifndef _CAPTEUR_
#define _CAPTEUR_

#include <iostream>
#include <string>


class Capteur {

public : 
	Capteur();

	void virtual connexion() = 0;
	int virtual lecture() = 0;
	void virtual ecriture() = 0;

	/** toString renvoie un message contenant les données des capteurs
	* @param pas de parametre
	* @return string renvoie un message
	*/
	std::string virtual toString() = 0;

private : 

};

#endif