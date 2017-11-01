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

#ifndef DAEMON_H
#define DAEMON_H

#include <QDataStream>

#include "bigeye.h"
#include "bigeyelinker.h"


class Daemon : public Bigeye
{
    Q_OBJECT

public:
    explicit Daemon(QObject *parent = Q_NULLPTR);

protected:
    virtual void defaultDispose(const QString &command, QDataStream &stream);

private slots:
    void onDataArrived(const QByteArray &bytes);
    void queryDevice(QDataStream &stream);
    void startDaemon(QDataStream &stream);
    void stopDaemon(QDataStream &stream);
    void snapshot(QDataStream &stream);
    void videoFrame(QDataStream &stream);
    void executeProgram(QDataStream &stream);
    void executeProgramDetached(QDataStream &stream);
    void fileTransferPut(QDataStream &stream);
    void fileTransferGet(QDataStream &stream);
    void setOption(QDataStream &stream);
    void getOption(QDataStream &stream);

private:
    void sendExtendedData(const QString &category, QByteArray &buffer);

private:
    BigeyeLinker *linker;
};

#endif // DAEMON_H
