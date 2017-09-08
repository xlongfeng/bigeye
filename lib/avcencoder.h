#ifndef AVCENCODER_H
#define AVCENCODER_H

#include <QThread>

#ifndef Q_NULLPTR
#define Q_NULLPTR         NULL
#endif

class AVCEncoder : public QThread
{
    Q_OBJECT
public:
    explicit AVCEncoder(QObject *parent = Q_NULLPTR);

    void stop();

protected:
    virtual void run();
};

#endif // AVCENCODER_H
