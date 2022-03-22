#include <QCoreApplication>
#include "source/Server.hpp"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server server;
    server.start_server();

    return a.exec();
}
