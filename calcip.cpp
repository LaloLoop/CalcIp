#include "calcip.h"
#include "ui_calcip.h"
#include "IPV4alidator.h"

calcIP::calcIP(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::calcIP)
{
    ui->setupUi(this);
    //Restricciones IP de entrada.
    IPV4Validator *ipValidator = new IPV4Validator;
    ui->ipLineEdit->setValidator(ipValidator);

    //Validador para el valor de la operación a realizar.
    QIntValidator *opRange = new QIntValidator(1,16777216,this);
    ui->opValue->setValidator(opRange);

    //ComboBox
    ui->opCombo->addItem("Hosts");
    ui->opCombo->addItem("Subredes");
    ui->opCombo->addItem("Prefijo");

    //Progressbar
    generator_progress = new QProgressBar(this);
    generator_progress->setTextVisible(false);
    generator_progress->setVisible(false);
    //Notificación de IP privada.
    ui->privateLabel->setVisible(false);
    ui->privateLabel->setToolTip("Esta dirección no puede ser usada como dirección pública.");

    //Propiedad privada especial.
    isPrivateSpecial = false;

    //Conexión de señales.
    connect(ui->ipLineEdit,SIGNAL(returnPressed()),this,SLOT(update_ip_info()));
    connect(ui->opValue,SIGNAL(returnPressed()),this,SLOT(calculate()));
    connect(ui->subnetsList,SIGNAL(itemSelectionChanged()),this,SLOT(update_hosts_list()));
//    connect(ui->ipLineEdit,SIGNAL(selectionChanged()))
    n_subnets = 0;
    statusBar()->showMessage("Listo",3000);
    statusBar()->addPermanentWidget(generator_progress);
}

calcIP::~calcIP()
{
    delete ui;
}

//Actualiza la información de la IP de entrada.
void calcIP::update_ip_info(){
    QString ip_string = ui->ipLineEdit->text();
    QStringList octets = ip_string.split(".");
    ip = new ipAddr(ip_string);
    qDebug() << "::input_IP_string:" << ip->toString();
    switch(ip->get_class()){
        case A:
            clase = "A";
            nmask ="255.0.0.0";
            break;
        case B:
            if(((octets.at(0).toInt() == 172) && ((octets.at(1).toInt() >= 17) && (octets.at(1).toInt() <= 31)))){
                QMessageBox::information(this,"Error! :(", "Ha ingresado una IP que hace referencia a un subred de '172.16.0.0'.\nIngrese una IP menor a '172.17.0.0' y mayor a '172.31.0.0'.");
                return;
            }
            if((octets.at(0).toInt() == 172) && (octets.at(1).toInt() == 16)){
                clase = "N/A";
                nmask = "255.240.0.0";
//                ui->privateLabel->setVisible(true);
                isPrivateSpecial = true;
                break;
            }
            clase = "B";
            nmask ="255.255.0.0";
            break;
        case C:
            clase = "C";
            nmask ="255.255.255.0";
            break;
        case D:
            clase = "D";
            nmask = "N/A";
            break;
        case E:
            clase = "E";
            nmask = "N/A";
            break;
        default:
            clase = "N/A";
            nmask = "N/A";
            break;
    }
    if(((octets.at(0).toInt() == 192) && octets.at(1).toInt() == 168) ||
            (octets.at(0).toInt() == 10) ||
            ((octets.at(0).toInt() == 172) && (octets.at(1).toInt() == 16))
            /*((octets.at(0).toInt() == 172) && ((octets.at(1).toInt() >= 16) && (octets.at(1).toInt() <= 31)))*/){
        ui->privateLabel->setVisible(true);
    }
    else{
        ui->privateLabel->setVisible(false);
    }
    ui->ipClassValue->setText(clase);
    ui->ipMaskValue->setText(nmask);
}

unsigned int calcIP::nearest_pow (unsigned int num)
{
    unsigned int n = num > 0 ? num - 1 : 0;

    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;

    return n;
}

//Calcula las operaciones requeridas.
void calcIP::calculate(){
    if(ui->ipLineEdit->text() == ""){
        QMessageBox::information(this,"Wait! :(",
                                 "Debe especificar una dirección IP.");
        return;
    }
    n_host_bits = 0;
    n_subnet_bits = 0;
    int prefix = 0;
    int opValue = ui->opValue->text().toInt();
    int n_host_bits_from_ip = ip -> get_host_bits();
    unsigned char nmask[4] = {0xff,0xff,0xff,0xff};
    unsigned *nmask_pt = (unsigned*)nmask;
    if(isPrivateSpecial){
        n_host_bits = 16;
        n_subnet_bits = 8;
        n_subnets = 1 << n_subnet_bits;
        n_hosts = ((1 << n_host_bits) - 2);
        return;
    }
    n_hosts = 1;
    if(ui->opCombo->currentIndex() != 2){
        unsigned int nearest = nearest_pow((unsigned int)opValue);
//        qDebug() << "::Nearest_pow_2:" << nearest;
        if((unsigned)opValue == nearest-1 || opValue == 1){
            opValue += 1;
        }
        //Log base 2?
        while(opValue != 0){
            n_host_bits += 1;
            n_hosts *= 2;
            opValue /= 2;
        }
     }
     if(n_host_bits > n_host_bits_from_ip){
        QMessageBox::information(this,"Wait! :(",
                                 "El valor de hosts debe ser menor a "
                                 + QString::number(pow(2,(double)n_host_bits_from_ip)));
    }
    switch(ui->opCombo->currentIndex()){
        case 1: //Subredes
            n_subnet_bits = (n_host_bits_from_ip - n_host_bits);
            n_subnets = ((1 << (n_subnet_bits)) - 2);
            //Prefijo.
            prefix = 32-(n_subnet_bits);
            //Nueva máscara de subred.
            *nmask_pt = (*nmask_pt << n_subnet_bits);
            nmask_final = new inetAddr(nmask);
            //Actualizando Widgets.
            ui -> hostsValue -> setText(QString::number(n_subnets));
            ui -> subnetsValue -> setText(QString::number(n_hosts));
            ui -> prefixValue -> setText(QString::number(prefix));
            ui->nmaskValue->setText(nmask_final->toString());
            //Actualizando lista de subredes.
            update_subnets_list();
        break;

        case 2: //Prefijo.
            if(opValue < ip->get_prefix_bits() || opValue >= 31){
                QMessageBox::information(this,"Wait! :(","El prefijo no debe ser menor a " +
                                         QString::number(ip->get_prefix_bits()) + " ni mayor a 31");
                return;
            }
            n_host_bits = 32 - opValue;
            n_subnet_bits = (n_host_bits_from_ip - n_host_bits);
            //Obtieniendo Número de hosts y de subredes.
            n_hosts = ((1 << n_host_bits) - 2); //2^n - 2
            n_subnets = (1 << n_subnet_bits);   //2^n
            //Obteniendo máscara de subred.
            *nmask_pt = *nmask_pt << n_host_bits;
            nmask_final = new inetAddr(nmask);
            //Actualizando Widgets.
            ui->hostsValue->setText(QString::number(n_hosts));
            ui->subnetsValue->setText(QString::number(n_subnets));
            ui->prefixValue->setText(QString::number(opValue));
            ui->nmaskValue->setText(nmask_final->toString());
            //Actualizando lista de subredes.
            update_subnets_list();
            break;

        default: //Hosts
            //N. de hosts.
            n_hosts -= 2;
            //N. de subredes.
            n_subnet_bits = (n_host_bits_from_ip - n_host_bits);
            n_subnets = (1 << (n_subnet_bits));
            //Prefijo.
            prefix = 32-n_host_bits;
            //Nueva máscara de subred.
            *nmask_pt = *nmask_pt << n_host_bits;
            nmask_final = new inetAddr(nmask);
            //Actualizando Widgets
            ui -> hostsValue -> setText(QString::number(n_hosts));
            ui -> subnetsValue -> setText(QString::number(n_subnets));
            ui -> prefixValue -> setText(QString::number(prefix));
            ui->nmaskValue->setText(nmask_final->toString());
            //Actualizando lista de subredes.
            update_subnets_list();
            break;
    }
}

void calcIP::update_subnets_list(void){
    int i, real_subnets;
    if(ui->subnetsList->count() != 0){
        ui->subnetsList->clear();
        ui->hostsList->clear();
    }
    if(ui->opCombo->currentIndex() == 1){
        real_subnets = n_hosts;
    }
    else{
        real_subnets = n_subnets;
    }
    inetAddr *var_ip = new inetAddr;
    unsigned char raw_ip[4];
    unsigned char tmp_ip[4];
    unsigned char nmask[4]={0xff,0xff,0xff,0xff};

    int * ip_pt = (int*) raw_ip;
    int * pt_tmp = (int*) tmp_ip;
    int * nmask_pt = (int*) nmask;

    //Formando máscara de la clase.
    ip->get_raw_address(raw_ip);
    *nmask_pt <<= ip->get_host_bits();

    //Obteniendo prefijo de la red.
    *ip_pt &= *nmask_pt;

    memcpy(tmp_ip,raw_ip,4);
    generator_progress->setRange(0,real_subnets-1);
    generator_progress->setVisible(true);
    for(i=0 ; i < real_subnets ; i++){
        *ip_pt |= *pt_tmp  ^ (((*pt_tmp >> n_host_bits) ^ (i)) << n_host_bits);
        var_ip -> set_address(raw_ip);
        ui -> subnetsList -> addItem(QString::number(i+1) + ". " + var_ip->toString());
        *ip_pt &= *nmask_pt;
        generator_progress->setValue(i);
    }
    generator_progress->setVisible(false);
}

void calcIP::update_hosts_list(){
    int i, real_hosts;
    if(ui->hostsList->count() != 0){
        ui->hostsList->clear();
    }
    if(ui->opCombo->currentIndex() == 1){
        real_hosts = n_subnets;
    }
    else{
        real_hosts = n_hosts;
    }
    inetAddr *var_ip = new inetAddr(ui->subnetsList->currentItem()->text().split(" ").at(1));
    qDebug() << "::Selected_IP:" << var_ip->toString();
    unsigned char raw_ip[4];
    unsigned char tmp_ip[4];
    unsigned char nmask[4];
    nmask_final -> get_raw_address(nmask);

    int * ip_pt = (int*) raw_ip;
    int * pt_tmp = (int*) tmp_ip;
    int * nmask_pt = (int*) nmask;

    var_ip->get_raw_address(raw_ip);

    memcpy(tmp_ip,raw_ip,4);

    *ip_pt &= *nmask_pt;

    generator_progress->setRange(0,real_hosts);
    generator_progress->setVisible(true);
    for(i=0 ; i < real_hosts ; i++){
        *ip_pt |= *pt_tmp  ^ (*pt_tmp ^ (i+1));
        var_ip -> set_address(raw_ip);
        ui -> hostsList -> addItem(QString::number(i+1) + ". " + var_ip->toString());
        *ip_pt &= *nmask_pt;
        generator_progress->setValue(i);
    }
    generator_progress->setVisible(false);
}
