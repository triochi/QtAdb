#ifndef SOCKETS_H
#define SOCKETS_H

#include <QtNetwork>

class Socket : public QObject
{
    Q_OBJECT
public:
    Socket(){}
    ~Socket();
    QTcpSocket *socket;
    unsigned int bytes_to_int(QByteArray data, int start);
    QString listFiles(QString path);
private:

private slots:
    void writeToSocket();
    void readFromSocket();
};
#endif // SOCKETS_H
