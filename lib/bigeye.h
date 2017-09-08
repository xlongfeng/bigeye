#ifndef BIGEYE_H
#define BIGEYE_H

#include <QObject>

#ifndef Q_NULLPTR
#define Q_NULLPTR         NULL
#endif

class Bigeye : public QObject
{
    Q_OBJECT
public:
    explicit Bigeye(QObject *parent = Q_NULLPTR);

protected:
    int getFramebufferWidth() const
    {
        return framebufferWidth;
    }
    int getFramebufferHeight() const
    {
        return framebufferHeight;
    }
    int getFramebufferBitDepth() const
    {
        return framebufferBitDepth;
    }
    int getFramebufferLength() const
    {
        return framebufferLength;
    }
    QByteArray& getFramebuffer();
    void dispose(const QByteArray &bytes);
    QByteArray escape(const QByteArray &data);
    QByteArray unescape(const QByteArray &data);

private:
    void getFramebufferInfo();

private:
    QByteArray datagram;
    int framebufferFd;
    int framebufferWidth;
    int framebufferHeight;
    int framebufferBitDepth;
    int framebufferLength;
    QByteArray framebuffer;
};

#endif // BIGEYE_H
