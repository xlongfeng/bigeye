#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#include <QDataStream>
#include <QDebug>

#include "bigeye.h"

Bigeye::Bigeye(QObject *parent) : QObject(parent)
{
    getFramebufferInfo();
}

void Bigeye::dispose(const QByteArray &bytes)
{
    for (int i = 0; i < bytes.size(); i++) {
        char ch = bytes.at(i);
        if (ch == 0x7e) {
            if (datagram.size() > 0) {
                const QByteArray &block = unescape(datagram);
                QDataStream istream(block);
                istream.setVersion(QDataStream::Qt_4_8);
                QString magic;
                QString command;
                istream >> magic >> command;
                if (magic == "Bigeye") {
                    QMetaObject::invokeMethod(this, command.toLatin1().constData(), Q_ARG(QDataStream &, istream));
                } else {
                    qDebug() << "Magic string error";
                }
                datagram.clear();
            }
        } else {
            datagram.append(ch);
            if (datagram.size() > 1024 * 1024 * 10) {
                datagram.clear();
            }
        }
    }
}

QByteArray Bigeye::escape(const QByteArray &data)
{
    QByteArray buf;
    buf.append(0x7e);
    for (int i = 0; i < data.size(); i++) {
        char ch = data.at(i);
        if (ch == 0x7e) {
            buf.append(0x7d);
            buf.append(0x5e);
        } else if (ch == 0x7d) {
            buf.append(0x7d);
            buf.append(0x5d);
        } else {
            buf.append(ch);
        }
    }
    buf.append(0x7e);
    return buf;
}

QByteArray Bigeye::unescape(const QByteArray &data)
{
    QByteArray buf;
    bool isEscaped = false;

    for (int i = 0; i < data.size(); i++) {
        char ch = data.at(i);
        if (isEscaped) {
            isEscaped = false;
            if (ch == 0x5e) {
                buf.append(0x7e);
            } else if (ch == 0x5d) {
                buf.append(0x7d);
            } else {
                qDebug() << "Data crrupted";
            }
        } else {
            if (ch == 0x7d) {
                isEscaped = true;
            } else {
                buf.append(ch);
            }
        }
    }
    return data;
}

QByteArray& Bigeye::getFramebuffer()
{
    lseek(framebufferFd, 0, SEEK_SET);
    read(framebufferFd, framebuffer.data(), framebuffer.size());
    return framebuffer;
}

void Bigeye::getFramebufferInfo()
{
    struct fb_var_screeninfo fb_varinfo;
    memset(&fb_varinfo, 0, sizeof(struct fb_var_screeninfo));

    const char *device = "/dev/fb0";
    framebufferFd = open(device, O_RDONLY);
    if (framebufferFd == -1) {
        qDebug() << "Error: Couldn't open" << device;
    }

    if (ioctl(framebufferFd, FBIOGET_VSCREENINFO, &fb_varinfo) != 0) {
        qDebug() << "ioctl FBIOGET_VSCREENINFO";
    }

    framebufferWidth = fb_varinfo.xres;
    framebufferHeight = fb_varinfo.yres;
    framebufferBitDepth = fb_varinfo.bits_per_pixel;
    framebufferLength = framebufferWidth * framebufferHeight * ((framebufferBitDepth + 7) >> 3);
    framebuffer.reserve(framebufferLength);
    framebuffer.resize(framebufferLength);
}
