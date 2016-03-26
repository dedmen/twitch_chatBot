#include "twitchhelper.h"

twitchHelper::twitchHelper()
{

}

twitchHelper::~twitchHelper()
{

}

QJsonObject twitchHelper::getUserChannelInfo(QString username)
{
	HTTPdownloader htdl;
	QByteArray data = htdl.Doget("https://api.twitch.tv/kraken/streams/"+username).toUtf8();
//qDebug() << data;
	QJsonObject obj = QJsonDocument::fromJson(data).object();
	return obj;
}

QJsonObject twitchHelper::getUserChannelInfo(QStringRef username)
{
	return getUserChannelInfo(*username.string());
}

QJsonObject twitchHelper::getBRData(QString username)
{
	HTTPdownloader htdl;
	QByteArray data = htdl.Doget("http://battleroyalegames.com/leaderboard/api.php?steamid="+username).toUtf8();
	QJsonObject obj = QJsonDocument::fromJson(data).object();
	return obj;
}

