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

#include <QDebug>

#include "launcher.h"


USBHandleEventThread::USBHandleEventThread(QObject *parent) : QThread(parent)
{

}

void USBHandleEventThread::run()
{
    qDebug() << "USBHandleEventThread" << "start";
    forever {
        libusb_handle_events_completed(Q_NULLPTR, Q_NULLPTR);
    }
    qDebug() << "USBHandleEventThread" << "end";
}


Launcher::Launcher(QObject *parent) : QObject(parent),
    ctx(Q_NULLPTR)
{
    int rc = libusb_init(&ctx);
    if (LIBUSB_SUCCESS != rc) {
        qDebug() << "libusb_init" << libusb_error_name(rc);
    }

    rc = libusb_hotplug_register_callback(
                ctx, (libusb_hotplug_event)(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED |
                LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
                (libusb_hotplug_flag)LIBUSB_HOTPLUG_ENUMERATE,
                0x2009, 0x0805, LIBUSB_HOTPLUG_MATCH_ANY,
                hotplug_callback, this, &hotplug);
    if (LIBUSB_SUCCESS != rc) {
        qDebug() << "libusb_init" << libusb_error_name(rc);
    }

    thread = new USBHandleEventThread(this);
    thread->start();
}

int Launcher::hotplug_callback(libusb_context *ctx, libusb_device *dev,
                            libusb_hotplug_event event, void *user_data)
{
    Launcher *launcher = (Launcher *)user_data;
    libusb_device_descriptor desc;
    int rc;

    libusb_get_device_descriptor(dev, &desc);

    if (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED == event) {
        rc = libusb_open(dev, &launcher->device);
        if (LIBUSB_SUCCESS != rc) {
            qDebug() << "libusb_open" << libusb_error_name(rc);
        } else {
            qDebug() << "libusb_open";
        }
    } else if (LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT == event) {
        if (launcher->device) {
            libusb_close(launcher->device);
            launcher->device = Q_NULLPTR;
        }
        qDebug() << "libusb_close";
    } else {
        qDebug() << "Unhandled event" << event;
    }

    return 0;
}
