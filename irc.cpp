#include "irc.h"
#include <QTcpSocket>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QtConcurrent/QtConcurrent>
//#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
#include "../LoadoutEditor/MainLibrary/httpdownloader.h"
#include "twitchhelper.h"
#include "localdata.h"
irc::irc(QObject *parent) :
	QObject(parent) {
	ready = false;
	buffer = QStringList();
	delay = 30;//Qt::BlockingQueuedConnection
	timer = new QTimer;
	regExp.PrivmsgExp.setPattern(R"(:([a-zA-Z0-9]*?)!.*? PRIVMSG #([\S]*?) :([\s\S]*?)[\r\n]+)");
	regExp.PrivmsgExp.optimize();
	regExp.JoinExp.setPattern(R"(:([a-zA-Z0-9]*?)!.*? JOIN #([\S]*?)[\r\n]+)");
	regExp.JoinExp.optimize();
	regExp.PartExp.setPattern(R"(:([a-zA-Z0-9]*?)!.*? JOIN #([\S]*?)[\r\n]+)");
	regExp.PartExp.optimize();
	regExp.ModeExp.setPattern(R"(:.*? MODE #([\S]*?) ([+-][a-z]) ([a-zA-Z0-9]*?)[\r\n])");
	regExp.ModeExp.optimize();

}
void irc::run() {
	//qDebug() << "IRCThreadStarted" << QThread::currentThreadId();
	socket = new QTcpSocket();
	connect(socket, SIGNAL(readyRead()), SLOT(readData()));

	// socket->moveToThread(QThread::currentThread());
	connectToSvr();
}

void irc::sendMessageToChannel(streamer* pStreamer, QString message, bool ignoreMute) {
	if (pStreamer == nullptr || (pStreamer->muted && !ignoreMute))
		return; //#TODO warning	on nullptr

	if (message.right(2).compare("\r\n"))
		message += "\r\n";
	//qDebug() << "send" << channel << message;

	QFile logfile("irclog.txt");
	if (logfile.open(QFile::Append | QFile::ReadWrite)) {
		logfile.write(QDateTime::currentDateTime().toString("yy-MM-dd_HH_mm_ss ").toUtf8());
		logfile.write(QString("PRIVMSG #" + pStreamer->channelId + " :" + message).toUtf8());
		logfile.close();
	}
	socket->write(QString("PRIVMSG #" + pStreamer->channelId + " :" + message).toUtf8());// immer \r\n am ende einer gesendeten nachricht
}

void irc::addChannel(QString channel) {
	if (channelList.contains(channel))
		return;

	channelList << channel;
	if (ready)
		socket->write(QString("JOIN #" + channel + "\r\n").toUtf8());
}

void irc::ThreadStarted() {
	//qDebug() << "IRCThreadStarted" << QThread::currentThreadId();
	socket = new QTcpSocket(this);
	connect(socket, SIGNAL(readyRead()), SLOT(readData()));
	connect(socket, SIGNAL(connected()), SLOT(connected()));
	// socket->moveToThread(thread);
}

irc::~irc() {
	if (ready) {
		socket->write("QUIT Good bye \r\n");
		socket->flush();
		socket->disconnectFromHost();
		socket->deleteLater();
	}
}

void irc::init(QString Server, int Port) {
	qDebug() << "InitIRC" << QThread::currentThreadId();
	IrcServer = Server;
	IrcPort = Port;
	// connect(GlobalVariablesO,SIGNAL(logMessage(QString)),SLOT(sendlog(QString)));
	socket = new QTcpSocket();
	connect(socket, SIGNAL(readyRead()), SLOT(readData()));
	connect(socket, SIGNAL(connected()), SLOT(connected()));
}

void irc::connectToSvr() {
	qDebug() << "ConnectIRC" << QThread::currentThreadId() << IrcServer << IrcPort;
	socket->connectToHost(IrcServer, IrcPort);
	socket->waitForConnected(3000);
	qDebug() << socket->state();

	connected();
	//socket->connectToHost("192.168.137.117", 4432);
}

void irc::connected() {
	qDebug() << "ConnectedIRC" << QThread::currentThreadId();
	if (socket->waitForConnected()) {
		socket->write(QString("PASS oauth:" + tmi_Pass + "\r\n").toUtf8());
		socket->write(QString("NICK " + tmi_User + "\r\n").toUtf8());
		socket->write(QString("USER GFBot bot " + tmi_Host + " :" + tmi_User + "\r\n").toUtf8());
		timer->setInterval(1000 + rand() % 600);
		timer->start();
	}
}

void irc::readData() {
	if (!socket->isReadable()) { return; }
	QByteArray data = socket->readAll();
	QFile logfile("irclog.txt");
	if (logfile.open(QFile::Append | QFile::ReadWrite)) {
		logfile.write(QDateTime::currentDateTime().toString("yy-MM-dd_HH_mm_ss ").toUtf8());
		logfile.write(data);
		logfile.close();
	}
	//qDebug() << data;
	QString dataS = QString(data);
	for (QString sp : dataS.split("\r\n")) {
		if (sp.length() < 3)
			continue;
		if (sp.right(2).compare("\r\n"))
			sp += "\r\n";
		parseData(sp);
	}
}

void irc::parseData(QString dataS) {
	if (dataS.contains("PRIVMSG", Qt::CaseSensitive)) {//on top because this is what will happen the most
		auto pMatch = regExp.PrivmsgExp.match(dataS);
		QString username = pMatch.captured(1);
		QString channel = pMatch.captured(2);
		QString message = pMatch.captured(3);
		emit ircMessageIn(channel, username, message);
		return;
	}
	if (dataS.contains("JOIN", Qt::CaseSensitive)) {
		/*
		auto pMatch = regExp.JoinExp.match(dataS);
		QString username = pMatch.captured(1);
		QString channel = pMatch.captured(2);
		*/
		//joined channel

		return;
	}
	if (dataS.contains("PART", Qt::CaseSensitive)) {
		/*
		auto pMatch = regExp.PartExp.match(dataS);
		QString username = pMatch.captured(1);
		QString channel = pMatch.captured(2);
		*/
		//left channel

		return;
	}
	if (dataS.contains("MODE", Qt::CaseSensitive)) {
		/*
		auto pMatch = regExp.ModeExp.match(dataS);
		QString channel = pMatch.captured(1);
		QString mode = pMatch.captured(2);
		QString username = pMatch.captured(3);

		//:jtv MODE #channel -o user
		//:jtv MODE #channel +o user
		*/
		//Mode change
		return;
	}




	if (dataS.contains("Checking Ident")) {
		socket->write(QString("NICK " + tmi_User + "\r\n").toUtf8());
		socket->write(QString("USER GFBot bot " + tmi_Host + " :" + tmi_User + "\r\n").toUtf8());
		return;
	}
	if (dataS.contains(QRegExp("(PING :)"))) {
		dataS.replace("PING", "PONG");
		socket->write(QString(dataS).toUtf8());
		return;
	}
	if (dataS.contains("Your host is tmi.twitch.tv")) {
		ready = true;
		for (QString str : channelList)
			socket->write(QString("JOIN #" + str + " \r\n").toUtf8());
		socket->write("CAP REQ :twitch.tv/membership\r\n");
		socket->write("CAP REQ :twitch.tv/commands\r\n");
		return;
	}
	if (dataS.contains("USERSTATE"))
		return;
	qDebug() << "unparsed " << dataS;
	//https://gist.github.com/3ventic/0c3282e4b62695da7203
}

