#include <QApplication>
#include <QTextCodec>
#include "calcip.h"
#include "ipaddr.h"

int main(int argc, char *argv[])
{
    //Coodificación del texto.
    QTextCodec *linuxCodec = QTextCodec::codecForName("UTF-8");
//    QTextCodec::setCodecForTr(linuxCodec);
//    QTextCodec::setCodecForCStrings(linuxCodec);
    QTextCodec::setCodecForLocale(linuxCodec);

    QApplication a(argc, argv);
    calcIP w;
    w.show();
    QString addr("192.168.1.72");
    return a.exec();
}
