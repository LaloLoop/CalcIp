#include "ipaddr.h"

ipAddr::ipAddr(QString ip) : inetAddr(ip)
{
    clase = calc_class();
    qDebug() << "::Clase:" << clase;
}

ip_class ipAddr::calc_class(void){
    //Verificación de clases.
    if(address[3] <= 127){
        prefix_bits = 8;
        host_bits = 24;
        return A;
    }
    if(address[3] >= 128 && address[3] <= 191){
        prefix_bits = 16;
        host_bits = 16;
        return B;
    }
    if(address[3] >= 192 && address[3] <= 223){
        prefix_bits = 24;
        host_bits = 8;
        return C;
    }
    if(address[3] >= 224 && address[3] <= 239){
        prefix_bits = 0;
        host_bits = 0;
        return D;
    }
    else{
        prefix_bits = 0;
        host_bits = 0;
        return E;
    }
}

ip_class ipAddr::get_class(void){
    return clase;
}

int ipAddr::get_host_bits(){
    return (int)host_bits;
}

int ipAddr::get_prefix_bits(){
    return prefix_bits;
}
