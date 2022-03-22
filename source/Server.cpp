#include "Server.hpp"
#include "../Protocol/Protocoll.hpp"
#include <QDebug>
#include <algorithm>


namespace
{
    const qint64 PORT{ 1234 };
}

Server::Server(QObject* parent)
{

}

void Server::start_server()
{
    m_server = new QTcpServer();

    connect(m_server, &QTcpServer::newConnection, this, &Server::new_client_connection);

    if (!m_server->listen(QHostAddress::Any, ::PORT))
    {
        qDebug() << "Failed start server" << m_server->errorString();
        return ;
    }

    qDebug() << "Start server";
}

void Server::socket_disconected()
{
    QTcpSocket* sender = qobject_cast<QTcpSocket*>(QObject::sender());

    std::remove_if(m_clients.begin(), m_clients.end(),
                   [&](QTcpSocket* socket)
                   {
                      return sender == socket;
                   });

    qDebug() << sender->peerAddress() << ": disconected";
}

void Server::socket_ready_read()
{    
    QTcpSocket* client = qobject_cast<QTcpSocket*>(QObject::sender());

    if (!client->isValid())
    {
        qDebug() << client->errorString();
        return;
    }

    std::string data = client->readAll().toStdString();
    qDebug() << data.c_str();

    Protocol::Type type = Protocol::switch_protocol_type(data);

    switch (type)
    {
        case Protocol::Type::HANDSHAKE:
        {
            Protocol::Hanshake handshake = Protocol::decode_handshake(data);
            std::string info{"sucsses"};
            send_all_client(Protocol::encode_info(Protocol::Info{handshake.user_name, info}));
            break;
        }
        case Protocol::Type::MESSAGE:
        {
            Protocol::Message message = Protocol::decode_message(data);
            send_all_client(Protocol::encode_message(message));
            break;
        }
        case Protocol::Type::INFO:
        {
            Protocol::Info info = Protocol::decode_info(data);
            send_all_client(Protocol::encode_info(info));
            break;
        }

        default:
        qDebug() << "None type protocol";
        break;
    }
}

void Server::new_client_connection()
{
    QTcpSocket* new_client = m_server->nextPendingConnection();

    connect(new_client, &QTcpSocket::disconnected, this, &Server::socket_disconected);
    connect(new_client, &QTcpSocket::readyRead, this, &Server::socket_ready_read);

    m_clients.push_back(new_client);

    qDebug() << new_client->peerName() << " : "<< new_client->peerAddress() << ": connected";
}

void Server::send_all_client(const std::string& data)
{
    for(QTcpSocket* socket : m_clients)
    {
        if (socket->isValid() && socket->isWritable())
        {
            socket->write(data.c_str());
        }
    }
}
