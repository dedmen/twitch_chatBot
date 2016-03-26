#include <QCoreApplication>
#include <irc.h>
irc* ircclient;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
	ircclient = new irc();
    ircclient->init("irc.twitch.tv",6667);
    ircclient->run();
    qDebug() << "Loop start";
    return a.exec();
}
