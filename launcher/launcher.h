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
