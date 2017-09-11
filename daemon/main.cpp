#include <QCoreApplication>

#include "daemon.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Daemon daemon;

    return a.exec();
}
