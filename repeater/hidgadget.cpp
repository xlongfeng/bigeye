#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Qt>

#include "hidgadget.h"

HidGadget *HidGadget::self = Q_NULLPTR;
static const char *devcie = "/dev/hidg0";

HidGadget::HidGadget(QObject *parent) : QObject(parent)
{
    fd = open(devcie, O_RDWR, 0666);

    if (fd < 0)
        perror(devcie);
}

HidGadget *HidGadget::instance()
{
    if (!self)
        self = new HidGadget();
    return self;
}

void HidGadget::report(int code, bool down)
{
    if (fd < 0)
        return;

    char data[8];
    int lenght = sizeof(data);
    memset(data, 0, lenght);

    if (down) {
        switch (code) {
        case Qt::Key_Escape:
            data[2] = 0x29;
            break;
        case Qt::Key_Tab:
            data[2] = 0x2b;
            break;
        case Qt::Key_Backspace:
            data[2] = 0x2a;
            break;
        case Qt::Key_Enter:
            data[2] = 0x28;
            break;
        case Qt::Key_Delete:
            data[2] = 0x4c;
            break;
        case Qt::Key_Left:
            data[2] = 0x50;
            break;
        case Qt::Key_Up:
            data[2] = 0x52;
            break;
        case Qt::Key_Right:
            data[2] = 0x4f;
            break;
        case Qt::Key_Down:
            data[2] = 0x51;
            break;
        case Qt::Key_Space:
            data[2] = 0x2c;
            break;
        default:
            if (code >= Qt::Key_F1 && code <= Qt::Key_F12) {
                data[2] = (code - Qt::Key_F1) + 0x3a;
            } else if (code >= Qt::Key_A && code <= Qt::Key_Z) {
                data[2] = (code - Qt::Key_A) + ('a' - 0x04);
            } else {

            }
            break;
        }
    }
    if (write(fd, data, lenght) != lenght) {
        perror(devcie);
    }
}
