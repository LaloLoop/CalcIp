#ifndef CALCIP_H
#define CALCIP_H

#include <QMainWindow>
//#include <QRegExp>
//#include <QRegExpValidator>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QProgressBar>
#include "ipaddr.h"
#include <math.h>


namespace Ui {
    class calcIP;
}

class calcIP : public QMainWindow
{
    Q_OBJECT

public:
    explicit calcIP(QWidget *parent = 0);
    ~calcIP();

private:
    //Ui
    Ui::calcIP *ui;

    //Para IP principal.
    ipAddr *ip;
    QString clase;
    QString nmask;
    bool isPrivateSpecial;

    //Para los cálculos.
    int n_hosts, n_subnets;
    inetAddr *nmask_final;
    int n_host_bits;
    int n_subnet_bits;
    QProgressBar *generator_progress;


    unsigned int nearest_pow (unsigned int num);
    void update_subnets_list(void);

private slots:
    void update_hosts_list(void);

public slots:
    void update_ip_info(); //Actualiza los valores de la IP original.
    void calculate(); //Calcula los valores pedidos.
};

#endif // CALCIP_H
