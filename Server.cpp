#include "Server.h"
#include <QDataStream>

Server::Server(){
    if(this->listen(QHostAddress::Any,2323)){
        emit seyconnect("start");
    }
    else{
       emit seyconnect("error");
    }
}

void Server::incomingConnection(qintptr socketDescriptor ){
    socket =new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this,&Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket,&QTcpSocket::deleteLater);
    counter_users++;
    Sockets.insert(counter_users, socket);

}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_2);
    if(in.status()== QDataStream::Ok)
    {
        QString str;
        in >> str;
        emit seyconnect(str);
        SendToClient(str);

    }
}

void Server::slotsendtoClient(QString mes)
{
    SendToClient(mes);
}

void Server::SendToClient(QString str){
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << str;

    for(auto it = Sockets.begin(); it!= Sockets.end() ;it++){
        it.value()->write(Data);

    }
}
