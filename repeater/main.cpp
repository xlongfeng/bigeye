#include <QCoreApplication>
#include "repeater.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Repeater repeater0(QLatin1String("ttyGS0"));
    Repeater repeater1(QLatin1String("ttyGS1"));
    QObject::connect(&repeater0, SIGNAL(dataArrived(QByteArray)),
                     &repeater1, SLOT(onDataArrived(QByteArray)));
    QObject::connect(&repeater1, SIGNAL(dataArrived(QByteArray)),
                     &repeater0, SLOT(onDataArrived(QByteArray)));

    return a.exec();
}
