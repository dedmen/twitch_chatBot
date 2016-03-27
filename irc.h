#ifndef IRC_H
#define IRC_H

#include <QThread>
#include <QTcpSocket>
#include <QString>
#include <QTimer>
#include "eventscheduler.h"
#include "database.h"
#include <QRegularExpression>
#include "streamer.h"
struct ircRegularExpressions {	   //just to group them together to not mess up class
	QRegularExpression PrivmsgExp;
	QRegularExpression JoinExp;
	QRegularExpression PartExp;
	QRegularExpression ModeExp;
};


class irc : public QObject {
	Q_OBJECT
public:
	explicit irc(QObject *parent = 0);
	~irc();
	void init(QString IrcServer, int IrcPort);
	void connectToSvr();
	void run();

private:
	QString IrcServer;
	int IrcPort;
	QTcpSocket *socket;
	bool ready;
	QStringList buffer;
	QTimer * timer;
	QStringList channelList;
	ircRegularExpressions regExp; //static to keep them optimized
	int delay;
	public slots:
	void addChannel(QString channel);
	void sendMessageToChannel(streamer* pStreamer, QString message,bool ignoreMute = false);
	private slots:
	void ThreadStarted();
	void connected();
	void readData();
	void parseData(QString dataS);
signals:
	void ircMessageIn(QString channel, QString user, QString message);

};
extern irc* ircclient;
#endif // IRC_H
