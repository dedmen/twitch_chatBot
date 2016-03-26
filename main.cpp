#include <QCoreApplication>
#include <controller.h>
controller* pControllerGlobal;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	pControllerGlobal = new controller();


	//ircclient->init("irc.twitch.tv",6667);
	//ircclient->run();
    qDebug() << "Loop start";
    return a.exec();
}
