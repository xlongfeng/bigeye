#include <QCoreApplication>

#include "launcher.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Launcher launcher;

    return a.exec();
}
