#include "Accelerometre.h"

Accelerometre::Accelerometre()
{
}

void Accelerometre::ouvrirBus(int& fd,const char I2C_BUS[])
{
    fd = open(I2C_BUS, O_RDWR);
    if (fd < 0)throw std::runtime_error("Erreur d'ouverture sur le bus I2C\n");

}

void Accelerometre::selectionAdresse(const int& fd, const int& I2C_ADDR)
{
    // Sélectionner l'adresse I2C de l'accéléromètre
    if (ioctl(fd, I2C_SLAVE, I2C_ADDR) < 0) throw std::runtime_error("Erreur de la selection de l'adresse du bus I2C\n");
}

void Accelerometre::ecriture(const int& fd, const int& registre, const int& commande)
{
    // Ecriture d'une commande sur un registre pour parametrer l'accelerometre
    if (i2c_smbus_write_byte_data(fd, registre, commande) < 0) throw std::runtime_error("Erreur d'ecriture sur le bus I2C\n");
}

int Accelerometre::lecture(const int& fd, const int& registre)
{
    int val = i2c_smbus_read_word_data(fd, registre);
    return val  < 0 ? throw std::runtime_error("Erreur de lecture sur le bus I2C\n") : val;
}

Accelerometre::~Accelerometre()
{
}
