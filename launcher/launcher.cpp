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

#include <QFile>
#include <QProcess>
#include "launcher.h"


Launcher::Launcher(QObject *parent) : Bigeye(parent)
{
    linker = new BigeyeLinker(this);
    connect(linker, SIGNAL(dataArrived(QByteArray)), this, SLOT(onDataArrived(QByteArray)));
    linker->start();
}

void Launcher::defaultDispose(const QString &command, QDataStream &stream)
{
    Q_UNUSED(stream)
    qDebug() << "Unhanded command:" << command;
}

void Launcher::onDataArrived(const QByteArray &bytes)
{
    dispose(bytes);
}

void Launcher::queryDevice(QDataStream &stream)
{
    Q_UNUSED(stream)
    QByteArray block;
    QDataStream istream(&block, QIODevice::WriteOnly);
    istream.setVersion(QDataStream::Qt_4_8);
    istream << QString("Bigeye");
    istream << QString("respQueryDevice");
    istream << QString("launcher")
            << QString("1.0")
            << getDeviceType();
    linker->tramsmitBytes(escape(block));
}

void Launcher::startDaemon(QDataStream &stream)
{
    Q_UNUSED(stream)
    QByteArray daemon;
    stream >> daemon;
    if (stream.status() == QDataStream::Ok) {
        QFile file("/tmp/bigeye-daemon");
        file.open(QIODevice::WriteOnly);
        file.setPermissions(file.permissions() | QFile::ExeOwner);
        file.write(daemon);
        file.close();
        linker->stop();
        QProcess::execute("/tmp/bigeye-daemon");
        linker->start();
    }
}

void Launcher::stopDaemon(QDataStream &stream)
{
    Q_UNUSED(stream)
}
