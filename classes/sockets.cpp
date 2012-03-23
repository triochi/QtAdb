#include "sockets.h"

unsigned int Socket::bytes_to_int(QByteArray data, int start)
{
    unsigned int result=0;
    unsigned char temp;
    temp = data[start + 3];
    result = result | temp;
    result = result << 8;
    temp = data[start + 2];
    result = result | temp;
    result = result << 8;
    temp = data[start + 1];
    result = result | temp;
    result = result << 8;
    temp = data[start];
    result = result | temp;

    return result;
}

Socket::~Socket()
{

}

QString Socket::listFiles(QString path)
{
    QByteArray data;
    int size, length, time, permissions;
    QString name;
    QString output;

    this->socket = new QTcpSocket();
    this->socket->connectToHost("127.0.0.1",5037,QTcpSocket::ReadWrite);
    if (this->socket->waitForConnected(2000))
    {
        this->socket->write("0012host:transport-any");
        this->socket->waitForReadyRead(2000);
        data = this->socket->read(4);
        if (data == "OKAY")
        {
            this->socket->write("0005sync:");
            this->socket->waitForReadyRead(2000);
            data = this->socket->read(4);
            if (data == "OKAY")
            {
                {
                    QByteArray ba;
                    ba.append('L');
                    ba.append('I');
                    ba.append('S');
                    ba.append('T');
                    ba.append((char)path.length());
                    ba.append((char)0);
                    ba.append((char)0);
                    ba.append((char)0);
                    ba.append(path);

                    this->socket->write(ba);
                    this->socket->waitForReadyRead(2000);
                    data = this->socket->readAll();
                    while (data.left(4) != "DONE")
                    {
                        if (data.size() < 21)
                        {
                            this->socket->waitForReadyRead(2000);
                            data.append(this->socket->readAll());
                        }
                        data.remove(0, 4);
                        permissions = bytes_to_int(data.left(4),0);
                        data.remove(0, 4);
                        size = bytes_to_int(data.left(4),0);
                        data.remove(0, 4);
                        time = bytes_to_int(data.left(4), 0);
                        data.remove(0, 4);
                        length = bytes_to_int(data.left(4),0);
                        data.remove(0, 4);
                        name = data.left(length);

                        output.append(QString("%1\t").arg(permissions, 0, 2));
                        output.append(QString("%1\t").arg(size, 0, 10));
                        output.append(QDateTime::fromTime_t(time).toString("MMM dd yyyy, hh:mm\t"));
                        output.append(name + "\n");
                        data.remove(0, length);
                    }
                }
            }
        }
    }
    this->socket->disconnectFromHost();
    delete socket;
    return output;
}

void Socket::writeToSocket()
{
//    QByteArray data;
//    int size, length, time, permissions;
//    QString name;
//    qint64 start = QDateTime::currentMSecsSinceEpoch();

//    this->socket = new QTcpSocket();
//    this->socket->connectToHost("127.0.0.1",5037,QTcpSocket::ReadWrite);
//    this->ui->radioButton_2->setChecked(true);
//    if (this->socket->waitForConnected(2000))
//    {
//        this->socket->write("0012host:transport-any");
//        this->socket->waitForReadyRead(2000);
//        data = this->socket->read(4);
//        if (data == "OKAY")
//        {
//            this->socket->write("0005sync:");
//            this->socket->waitForReadyRead(2000);
//            data = this->socket->read(4);
//            if (data == "OKAY")
//            {
//                if (this->ui->radioButton->isChecked())
//                {
//                    QByteArray ba;
//                    ba.append('S');
//                    ba.append('T');
//                    ba.append('A');
//                    ba.append('T');
//                    ba.append((char)this->ui->lineEdit->text().length());
//                    ba.append((char)0);
//                    ba.append((char)0);
//                    ba.append((char)0);
//                    ba.append(this->ui->lineEdit->text());

//                    this->socket->write(ba);
//                    this->socket->waitForReadyRead(2000);
//                    data = this->socket->readAll();
//                    for (int i=0; i<data.size(); i++)
//                    {
//                        this->ui->plainTextEdit->insertPlainText(QString::number(data.at(i), 16));
//                        this->ui->plainTextEdit->insertPlainText(" ");
//                    }
//                }
//                else if (this->ui->radioButton_2->isChecked())
//                {
//                    QByteArray ba;
//                    ba.append('L');
//                    ba.append('I');
//                    ba.append('S');
//                    ba.append('T');
//                    ba.append((char)this->ui->lineEdit->text().length());
//                    ba.append((char)0);
//                    ba.append((char)0);
//                    ba.append((char)0);
//                    ba.append(this->ui->lineEdit->text());

//                    this->socket->write(ba);
//                    this->socket->waitForReadyRead(2000);
//                    data = this->socket->readAll();
//                    while (data.left(4) != "DONE")
//                    {
//                        if (data.size() < 21)
//                        {
//                            this->socket->waitForReadyRead(2000);
//                            data.append(this->socket->readAll());
//                        }
//                        data.remove(0, 4);
//                        permissions = bytes_to_int(data.left(4),0);
//                        data.remove(0, 4);
//                        size = bytes_to_int(data.left(4),0);
//                        data.remove(0, 4);
//                        time = bytes_to_int(data.left(4), 0);
//                        data.remove(0, 4);
//                        length = bytes_to_int(data.left(4),0);
//                        data.remove(0, 4);
//                        name = data.left(length);

//                        this->ui->plainTextEdit->insertPlainText(QString(" %1 ").arg(permissions, 16, 2));
//                        this->ui->plainTextEdit->insertPlainText(QString(" - %1 - ").arg(size, 10, 10));
//                        this->ui->plainTextEdit->insertPlainText(QDateTime::fromTime_t(time).toString("MMM dd yyyy, hh:mm - "));
//                        this->ui->plainTextEdit->insertPlainText(name + "\n");
//                        data.remove(0, length);
//                    }
//                }
//            }
//        }
//    }
}

void Socket::readFromSocket()
{

}

