/*
 * Bigeye - Accessorial Tool for Daily Test
 *
 * Copyright (c) 2017, longfeng.xiao <xlongfeng@126.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QMetaObject>
#include <QProcess>
#include <QFile>
#include <QDebug>

#include "daemon.h"
#include "option.h"


Daemon::Daemon(QObject *parent) : Bigeye(parent)
{
    linker = new BigeyeLinker(this);
    connect(linker, SIGNAL(dataArrived(QByteArray)), this, SLOT(onDataArrived(QByteArray)));
    linker->start();
}

void Daemon::defaultDispose(const QString &command, QDataStream &stream)
{
    Q_UNUSED(stream)
    qDebug() << "Unhanded command:" << command;
}

void Daemon::onDataArrived(const QByteArray &bytes)
{
    dispose(bytes);
}

void Daemon::queryDevice(QDataStream &stream)
{
    Q_UNUSED(stream)
    QByteArray block;
    QDataStream istream(&block, QIODevice::WriteOnly);
    istream.setVersion(QDataStream::Qt_4_8);
    istream << QString("Bigeye");
    istream << QString("respQueryDevice");
    istream << QString("daemon")
            << QString("1.0")
            << getDeviceType()
            << getFramebufferWidth()
            << getFramebufferHeight()
            << getFramebufferBitDepth();

    linker->tramsmitBytes(escape(block));
}

void Daemon::startDaemon(QDataStream &stream)
{
    Q_UNUSED(stream)
}

void Daemon::stopDaemon(QDataStream &stream)
{
    Q_UNUSED(stream)
}

void Daemon::snapshot(QDataStream &stream)
{
    Q_UNUSED(stream)

    QByteArray &buffer = getFramebuffer();
    sendExtendedData("snapshot", buffer);
}

void Daemon::videoFrame(QDataStream &stream)
{
    Q_UNUSED(stream)

    QByteArray &buffer = getFramebuffer();
    sendExtendedData("videoFrame", buffer);
}

void Daemon::executeProgram(QDataStream &stream)
{
    QString program;
    QStringList arguments;
    stream >> program;
    stream >> arguments;
    if (stream.status() == QDataStream::Ok) {
        QProcess::execute(program, arguments);
    }
}

void Daemon::executeProgramDetached(QDataStream &stream)
{
    QString program;
    QStringList arguments;
    stream >> program;
    stream >> arguments;
    if (stream.status() == QDataStream::Ok) {
        QProcess::startDetached(program, arguments);
    }
}

void Daemon::fileTransferPut(QDataStream &stream)
{
    QString src;
    QString dst;
    QByteArray data;
    stream >> src >> dst >> data;
    if (stream.status() == QDataStream::Ok) {
        QFile file(dst);
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.close();
    }
}

void Daemon::fileTransferGet(QDataStream &stream)
{
    QString src;
    QString dst;
    QByteArray data;
    stream >> src >> dst;
    if (stream.status() == QDataStream::Ok) {
        QFile file(src);
        file.open(QIODevice::ReadOnly);
        data = file.readAll();
        file.close();

        QByteArray block;
        QDataStream istream(&block, QIODevice::WriteOnly);
        istream.setVersion(QDataStream::Qt_4_8);
        istream << QString("Bigeye");
        istream << QString("respFileTransferGet");
        istream << src << dst << data;

        linker->tramsmitBytes(escape(block));
    }
}

void Daemon::setOption(QDataStream &stream)
{
    QString name, value;
    stream >> name >> value;
    option->setenv(name.toLatin1().data(), value.toLatin1().data());
    option->saveenv();
}

void Daemon::getOption(QDataStream &stream)
{
    QString name;
    stream >> name;
    QString value = option->getenv(name.toLatin1().data());

    QByteArray block;
    QDataStream istream(&block, QIODevice::WriteOnly);
    istream.setVersion(QDataStream::Qt_4_8);
    istream << QString("Bigeye");
    istream << QString("respGetOption");
    istream << name << value;

    linker->tramsmitBytes(escape(block));
}

void Daemon::sendExtendedData(const QString &category, QByteArray &buffer)
{
    QByteArray block;
    QDataStream istream(&block, QIODevice::WriteOnly);
    istream.setVersion(QDataStream::Qt_4_8);
    istream << QString("Bigeye") << QString("extendedData");
    istream << category;
    bool compressed = false;
    if (compressed) {
        buffer = qCompress(buffer);
    }
    istream << compressed << buffer.size();
    linker->tramsmitBytes(escape(block));
    linker->tramsmitBytes(buffer);
}
