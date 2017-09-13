#include <QCoreApplication>
#include "repeater.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Repeater repeater0(QLatin1String("ttyGS0"), true);
    Repeater repeater1(QLatin1String("ttyGS1"));
    QObject::connect(&repeater0, SIGNAL(dataArrived(const char*,int)),
                     &repeater1, SLOT(onDataArrived(const char*,int)));
    QObject::connect(&repeater1, SIGNAL(dataArrived(const char*,int)),
                     &repeater0, SLOT(onDataArrived(const char*,int)));

    return a.exec();
}
