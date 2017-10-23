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

#ifndef REPEATER_H
#define REPEATER_H

#include <QDataStream>

#include "bigeye.h"

class QTimer;
class QextSerialPort;

class Repeater : public Bigeye
{
    Q_OBJECT
public:
    explicit Repeater(const QString &portName, bool isStudio = false, QObject *parent = Q_NULLPTR);

signals:
    void dataArrived(const char *data, int length);

public slots:
    void onDataArrived(const char *data, int length);

private slots:
    void onConnectTimeout();
    void onPollTimeout();
    void onReadyRead();
    void startDaemon(QDataStream &stream);
    void stopDaemon(QDataStream &stream);
    void keyEventOpen(QDataStream &stream);
    void keyEventClose(QDataStream &stream);
    void keyEvent(QDataStream &stream);
    void extendedData(QDataStream &stream);
#if 0
    void snapshot(QDataStream &stream);
    void videoFrame(QDataStream &stream);
    void executeProgram(QDataStream &stream);
    void executeProgramDetached(QDataStream &stream);
    void fileTransferPut(QDataStream &stream);
    void fileTransferGet(QDataStream &stream);
#endif

private:
    void sendExtendedData(const QString &category, QByteArray &buffer);

private:
    bool isStudio;
    QextSerialPort *port;
    QTimer *connectTimer;
    QTimer *pollTimer;
    QByteArray buffer;
    static const int BufferSize = 128 * 1024;
};

#endif // REPEATER_H
