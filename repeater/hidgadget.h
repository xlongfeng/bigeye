#ifndef HIDGADGET_H
#define HIDGADGET_H

#include <QObject>

#ifndef Q_NULLPTR
#define Q_NULLPTR         NULL
#endif

class HidGadget : public QObject
{
    Q_OBJECT

public:
    static HidGadget *instance();

    void report(int code, bool down);

private:
    explicit HidGadget(QObject *parent = Q_NULLPTR);
    Q_DISABLE_COPY(HidGadget)

private:
    static HidGadget *self;

    int fd;
};

#endif // HIDGADGET_H
