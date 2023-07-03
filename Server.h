#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QMap>

class Server : public QTcpServer{
    Q_OBJECT
public:
    Server();
    QTcpSocket *socket;
    void printMes(QString mes);

private:
    QMap<int, QTcpSocket*> Sockets;
    QByteArray Data;
    void SendToClient(QString str);
    int counter_users=0;
signals:
    void seyconnect(QString mes);
    void printMessage(QString message);

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
    void slotsendtoClient(QString mes);
};

#endif // SERVER_H
