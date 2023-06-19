#pragma once
#include "serialib.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <vector>
#include <sstream>
class GestionBalance
{
private :
	serialib serial;
	uint baud; 
	std::string port;
	std::string poids; //la balance retourne un float avec deux chiffres significatifs * 100 pour avoir un int short car valeur max = 327,67 voir si possible de le changer pour un char par exemple 
public :
	GestionBalance();
	GestionBalance(const std::string& port, const int& baud);
	int connexion();
	void lecture();
	int fermeture();
	void mutateurPoids(std::string poids);
	std::string accesseurPoids();
	std::vector<std::string> split(const std::string& s, char delimiter);
	std::string decEnHex(int dec);
	std::string toString();
};

