#ifndef SERVER_HPP
#define SERVER_HPP

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <vector>
#include <string>


class Server : public QObject
{
    Q_OBJECT
public:
    Server(QObject* parent = nullptr);
    void start_server();
    void socket_disconected();
    void socket_ready_read();

public slots:
    void new_client_connection();

private:
    void send_all_client(const std::string& data);

private:
    QTcpServer* m_server;
    std::vector<QTcpSocket*> m_clients;

};

#endif // SERVER_HPP
