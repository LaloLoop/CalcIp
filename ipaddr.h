#ifndef IPADDR_H
#define IPADDR_H

#include "inetaddr.h"
#include <QString>
#include <QDebug>

//Clases permitidas.
enum ip_class {A,B,C,D,E};

class ipAddr : public inetAddr
{
protected:
    ip_class clase;
    unsigned int prefix_bits : 5;
    unsigned char host_bits;
    //Funciones inicializadoras.
    ip_class calc_class(void);

public:
    ipAddr(QString);
    ip_class get_class(void);
    int get_prefix_bits();
    int get_host_bits();
};

#endif // IPADDR_H
