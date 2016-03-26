#ifndef IRC_H
#define IRC_H

#include <QThread>
#include <QTcpSocket>
#include <QString>
#include <QTimer>
#include "eventscheduler.h"
#include "database.h"
class irc : public QObject
{
		Q_OBJECT
	public:
		explicit irc(QObject *parent = 0);
		~irc();
		void init(QString IrcServer,int IrcPort);
		void connectToSvr();
		void run();
		eventScheduler evSched;
		database db;
	private:
		QString IrcServer;
		int IrcPort;
		QTcpSocket *socket;
		bool ready;
		QStringList buffer;
		QTimer * timer;
		QStringList channelList;
		int delay;
	private slots:
		void ThreadStarted();
		void connected();
		void readData();
		void parseData (QString dataS);
		void sendMessageToChannel(QString channel,QString message);
};
extern irc* ircclient;
#endif // IRC_H
