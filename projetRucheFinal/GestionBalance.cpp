#include "GestionBalance.h"

GestionBalance::GestionBalance()
{
}

GestionBalance::GestionBalance(const std::string& port, const int& baud)
{
	this->port = port;
	this->baud = baud;
	this->poids = poids="0";
}

std::vector<std::string> GestionBalance::split(const std::string& s, char delimiter)
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

int GestionBalance::connexion()
{
	int erreur = serial.Open(port.c_str(), baud);
	return erreur;
}

void GestionBalance::lecture()//lecture de la trame envoyée par la balance 
{
	char recu[500];
	float retour = serial.Read(recu,'\n', 1000); //reception de la trame
	std::string trameRMC(recu);
	std::vector<std::string> morceauTrameRMC = split(trameRMC, ' '); // Split car espace changeant
	std::string payLoad = morceauTrameRMC.at(morceauTrameRMC.size() - 1);// Essai pour afficher une partie spécifique de la trame
	morceauTrameRMC = split(payLoad, 'k'); // Split
	payLoad = morceauTrameRMC.at(0);// Essai pour afficher une partie spécifique de la trame
	poids = static_cast<int>(stof(payLoad));//sinon perte de précision dû au .
	mutateurPoids(payLoad);
}

int GestionBalance::fermeture()
{
	serial.Close();
	return 0;
}

void GestionBalance::mutateurPoids(std::string poids)
{
	this->poids = poids;
}

std::string GestionBalance::accesseurPoids()
{
	//std::string poidsString = std::to_string(poids);
	return poids;
}
std::string GestionBalance::decEnHex(int dec)
{
	std::string hex, temp;
	int div;
	while (dec > 0) {
		div = dec % 16;
		if (div <= 9) temp += (div + '0');
		else temp += (div - 10 + 'A');
		dec = dec / 16;
	}
	int size = temp.size();
	for (int i = 0; i < size; i++) {
		hex += temp.at(size - i - 1);
	}
	return hex;
}

std::string GestionBalance::toString()
{
	std::string message = "Masse : " + poids + " Kg\n";
	return message;
}
