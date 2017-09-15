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

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <libusb-1.0/libusb.h>

#include <QObject>
#include <QThread>
#include <QMutex>

#ifndef Q_NULLPTR
#define Q_NULLPTR         NULL
#endif

class USBHandleEventThread: public QThread
{
    Q_OBJECT

public:
    explicit USBHandleEventThread(QObject *parent = Q_NULLPTR);

protected:
    virtual void run();

};

class Launcher : public QObject
{
    Q_OBJECT

public:
    explicit Launcher(QObject *parent = Q_NULLPTR);

private:
    static int hotplug_callback(libusb_context *ctx, libusb_device *dev,
                                libusb_hotplug_event event, void *user_data);

private:
    QMutex mutex;

    libusb_context *ctx;
    libusb_device_handle *device;
    libusb_hotplug_callback_handle hotplug;
    USBHandleEventThread *thread;
};

#endif // LAUNCHER_H
