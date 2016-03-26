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
	QObject(parent)
{
	ready = false;
	buffer = QStringList();
	delay = 30;//Qt::BlockingQueuedConnection
	timer = new QTimer;
	channelList << "cyngii" << "m0hawpi" << "dedmenmiller" /*<< "r0nond3x"*/ << "frappuccino1337" /*<< "ewanng"*/;
	connect(&evSched,&eventScheduler::sendMessageToChannel,this,&irc::sendMessageToChannel);
}
void irc::run(){
	//qDebug() << "IRCThreadStarted" << QThread::currentThreadId();
	socket = new QTcpSocket();
	connect(socket, SIGNAL(readyRead()),SLOT(readData()));


	//QFile json("E:/dbg/cyngii.json");
	//json.open(QFile::ReadOnly);
	//QByteArray data = json.readAll();
	//json.close();
	//HTTPdownloader htdl;
	//QByteArray data = htdl.Doget("https://api.twitch.tv/kraken/streams/cyngii").toUtf8();
	//qDebug() << data;
	//QJsonObject obj = twitchHelper::getUserChannelInfo("cyngii");//QJsonDocument::fromJson(data).object();
	//qDebug() << obj.value("stream").toObject().value("created_at").toString();
	//QDateTime time = QDateTime::fromString(obj.value("stream").toObject().value("created_at").toString(),"yyyy-MM-ddTHH:mm:ssZ");
	//time = time.addSecs(60*60);
	//
	//qDebug() <<"t"  << time.toString() << time << time.secsTo(QDateTime::currentDateTime())/60;




	// socket->moveToThread(QThread::currentThread());
	connectToSvr();
}

void irc::sendMessageToChannel(QString channel, QString message)
{

	if (message.right(2).compare("\r\n"))
		message += "\r\n";
	//qDebug() << "send" << channel << message;

	QFile logfile("irclog.txt");
	if (logfile.open(QFile::Append | QFile::ReadWrite)){
		logfile.write(QDateTime::currentDateTime().toString("yy-MM-dd_HH_mm_ss ").toUtf8());
		logfile.write(QString("PRIVMSG #"+channel+" :"+message).toUtf8());
		logfile.close();
	}
	socket->write(QString("PRIVMSG #"+channel+" :"+message).toUtf8());// immer \r\n am ende einer gesendeten nachricht
}

void irc::ThreadStarted(){
	//qDebug() << "IRCThreadStarted" << QThread::currentThreadId();
	socket = new QTcpSocket(this);
	connect(socket, SIGNAL(readyRead()),SLOT(readData()));
	connect (socket,SIGNAL(connected()),SLOT(connected()));
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
	connect(socket, SIGNAL(readyRead()),SLOT(readData()));
	connect(socket, SIGNAL(connected()),SLOT(connected()));
}

void irc::connectToSvr(){
	qDebug() << "ConnectIRC" << QThread::currentThreadId() << IrcServer << IrcPort;
	socket->connectToHost(IrcServer,IrcPort);
	socket->waitForConnected(3000);
	qDebug() << socket->state();

	connected();
	//socket->connectToHost("192.168.137.117", 4432);
}

void irc::connected(){
	qDebug() << "ConnectedIRC" << QThread::currentThreadId();
	if( socket->waitForConnected() ) {
		socket->write(QString("PASS oauth:"+tmi_Pass+"\r\n").toUtf8());
		socket->write(QString("NICK "tmi_User"\r\n").toUtf8());
		socket->write(QString("USER GFBot bot "tmi_Host" :"+tmi_User+"\r\n").toUtf8());
		timer->setInterval(1000+rand()%600);
		timer->start();
	}
}

void irc::readData(){
	if (!socket->isReadable()) {return;}
	QByteArray data = socket->readAll();
	QFile logfile("irclog.txt");
	if (logfile.open(QFile::Append | QFile::ReadWrite)){
		logfile.write(QDateTime::currentDateTime().toString("yy-MM-dd_HH_mm_ss ").toUtf8());
		logfile.write(data);
		logfile.close();
	}
	qDebug() << data;
	QString dataS = QString(data);
	for (QString sp : dataS.split("\r\n")){
		if (sp.right(2).compare("\r\n"))
			sp += "\r\n";
		parseData(sp);
	}
}

void irc::parseData(QString dataS)
{
	if (dataS.contains("Checking Ident")){
		socket->write(QString("NICK "tmi_User"\r\n").toUtf8());
		//socket->flush();
		socket->write(QString("USER GFBot bot "tmi_Host" :"+tmi_User+"\r\n).toUtf8());
		// socket->flush();
	}
	if (dataS.contains(QRegExp("(PING :)"))){
		dataS.replace("PING","PONG");
		socket->write(QString(dataS).toUtf8());
		//socket->flush();
	}
	if (dataS.contains("Your host is tmi.twitch.tv")){
		for (QString str : channelList)
			socket->write(QString("JOIN #"+str+" \r\n").toUtf8());
		socket->write("CAP REQ :twitch.tv/membership\r\n");
		evSched.registerBrEvent("76561198052890618","cyngii");
		evSched.registerBrEvent("76561198125252204","frappuccino1337");
		evSched.registerStreamStatEvent("cyngii");
		evSched.registerStreamStatEvent("dedmenmiller");
		//socket->flush();
	}
	//if ((dataS.contains("PART #cyngii") || dataS.contains("JOIN #cyngii")) && !dataS.toLower().contains("M0hawpsi")){
	//    dataS = dataS.right(dataS.size()-1);
	//    qDebug() << "PART" << dataS;
	//    dataS = dataS.left(dataS.indexOf("!"));
	//      qDebug() << "PART" << dataS;
	//      QString message = QString("PRIVMSG #theshadow451 :Hallo "+dataS+"\r\n");
	//      qDebug() << "message" << message;
	//            socket->write(message.toUtf8());
	//}
	//    qDebug() << dataS;
	int indexOfPrivMsg = dataS.indexOf("PRIVMSG #",Qt::CaseInsensitive);
	if (indexOfPrivMsg != -1) {
		int begin = indexOfPrivMsg+sizeof("PRIVMSG #")-1;
		int end = dataS.indexOf(" ",indexOfPrivMsg+sizeof("PRIVMSG #"));
		QString channel =dataS.mid( begin,end-begin);
		qDebug() << "channelmsg" << channel;

		dataS = dataS.right(dataS.length()-dataS.indexOf("PRIVMSG #"+channel+" :",0,Qt::CaseInsensitive)-17);
		//dataS = nachricht im channel gesendet
		if (dataS.contains("!dedmen"))
			sendMessageToChannel(channel,"WHO SAID THAT?!??!?!!!\r\n");
		if (dataS.contains("!viewers")){

			QJsonObject obj = twitchHelper::getUserChannelInfo(channel);
			//qDebug() << obj.value("stream").toObject().value("viewers").toInt();
			sendMessageToChannel(channel,"We currently have "+QString::number(obj.value("stream").toObject().value("viewers").toInt())+" viewers\r\n");
		}
		if (dataS.contains("!live") ||dataS.contains("!uptime") ){
			QJsonObject obj = twitchHelper::getUserChannelInfo(channel);
			qDebug() << obj.value("stream").toObject().value("created_at").toString();
			QDateTime time = QDateTime::fromString(obj.value("stream").toObject().value("created_at").toString(),"yyyy-MM-ddTHH:mm:ssZ");
			time = time.addSecs(60*60);
			sendMessageToChannel(channel,"We are live for "+QString::number(time.secsTo(QDateTime::currentDateTime())/60)+" minutes\r\n");
		}
		if (dataS.contains("!BR",Qt::CaseInsensitive) ||dataS.contains("!rank",Qt::CaseInsensitive) ){
			QString steamId;
			for (eventBase* ev : evSched.getEvents()){
				event_BRRankup* brev = dynamic_cast<event_BRRankup*>(ev);
				if (brev == nullptr)
					continue;
				if (channel.compare(brev->getChannel()) == 0){
					steamId = brev->getSteamID();
					break;
				}
			}
			QJsonObject obj = twitchHelper::getBRData(steamId);
			int kills = obj.value("kills").toInt();
			int losses = obj.value("losses").toInt();
			int rank = obj.value("global_rank").toInt();


			QString brrank;
			if (channel.compare("cyngii") == 0)
				brrank= QString("My Master has been awarded with rank %1 for his %2 Kills and %3 Deaths.").arg(QString::number(rank),QString::number(kills),QString::number(losses));
			else
				brrank= QString("%1 has been awarded with rank %2 for his %3 Kills and %4 Deaths.").arg(channel,QString::number(rank),QString::number(kills),QString::number(losses));
			sendMessageToChannel(channel,brrank);
		}

	}
}

