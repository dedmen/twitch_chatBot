#include "event.h"
#include "twitchhelper.h"
#include <QDebug>
#include "irc.h"
eventBase::eventBase(streamer* pStreamer) : owner(pStreamer)
{

}

eventBase::~eventBase()
{

}

event_BRRankup::event_BRRankup(streamer* pStreamer) : eventBase(pStreamer)
{
	this->steamID = pStreamer->getConfigValue(""); //#FIXME
	this->channel = channel;
	lastRank = 0;
	lastKills = 0;
	lastLosses = 0;
}

event_BRRankup::~event_BRRankup()
{




}

void event_BRRankup::tickEvent()
{
	qDebug() << __FUNCTION__;
	QJsonObject obj = twitchHelper::getBRData(steamID);
	int kills = obj.value("kills").toInt();
	int losses = obj.value("losses").toInt();
	int rank = obj.value("global_rank").toInt();
	if (rank == 0) //data get error or somethin
		return;
	if (rank != lastRank){ //Rankchange

		if (lastRank == 0){
			qDebug() << "rankSet";
			lastRank = rank;
			lastKills = kills;
			lastLosses = losses;
			return;
		} else if (rank > lastRank && (lastKills != kills || lastLosses != losses)){ //Kills und losses so that it doesnt message when another player gets higher rank and pushes target down
			qDebug() << "rankDown";
			if (channel.compare("cyngii") == 0)
				emit sendMessageToChannel(channel, QString("BattleRoyale Downrank ._. My Master has been punished from rank %1 to rank %2 for his %3 Kills and %4 Deaths.").arg(QString::number(lastRank),QString::number(rank),QString::number(kills),QString::number(losses)));
			else
				emit sendMessageToChannel(channel, QString("BattleRoyale Downrank ._. %1 has been punished from rank %2 to rank %3 for his %4 Kills and %5 Deaths.").arg(channel,QString::number(lastRank),QString::number(rank),QString::number(kills),QString::number(losses)));

		} else if (rank < lastRank && (lastKills != kills || lastLosses != losses)) {
			qDebug() << "rankUp";
			if (channel.compare("cyngii") == 0)
				emit sendMessageToChannel(channel, QString("BattleRoyale Uprank! My Master has been awarded from rank %1 to rank %2 for his %3 Kills and %4 Deaths.").arg(QString::number(lastRank),QString::number(rank),QString::number(kills),QString::number(losses)));
			else
				emit sendMessageToChannel(channel, QString("BattleRoyale Uprank! %1 has been awarded from rank %2 to rank %3 for his %4 Kills and %5 Deaths.").arg(channel,QString::number(lastRank),QString::number(rank),QString::number(kills),QString::number(losses)));

		}
	}
	lastRank = rank;
	lastKills = kills;
	lastLosses = losses;
}
QString event_BRRankup::getSteamID() const
{
	return steamID;
}

QString event_BRRankup::getChannel() const
{
	return channel;
}

event_streamStats::event_streamStats(streamer* pStreamer) : eventBase(pStreamer)
{
	this->channel = channel;
	this->recordViewers = ircclient->db.getStreamerValue(channel,"viewerRecord").toInt();
	this->recordUptime =ircclient->db.getStreamerValue(channel,"uptimeRecord").toULongLong();
	qDebug() << __FUNCTION__ << channel << recordViewers << recordUptime;
	lastUptime = 0;
	lastViewers = 0;
	viewerAlertIssued = false;
	uptimeAlertIssued = false;
	failcount = -1;
}

event_streamStats::~event_streamStats()
{

}
#include <QJsonDocument>
void event_streamStats::tickEvent()
{
	qDebug() << __FUNCTION__;
	QJsonObject obj = twitchHelper::getUserChannelInfo(channel);


	if (obj.value("stream").isNull()){
		if (failcount != -1)
			++failcount;
		if (failcount < 3)
			return;
		//qDebug() << QJsonDocument(obj).toJson();
		failcount = -1;
		viewerAlertIssued = false;
		uptimeAlertIssued = false;
		//if (lastViewers > recordViewers){
		ircclient->db.setStreamerValue(channel,"viewerRecord",QString::number(recordViewers));
		//recordViewers = lastViewers;
		//}

		//if (lastUptime > recordUptime){
		ircclient->db.setStreamerValue(channel,"uptimeRecord",QString::number(recordUptime));
		//recordUptime = lastUptime;
		//}
		return;
	}
	failcount = 0;


	QDateTime time = QDateTime::fromString(obj.value("stream").toObject().value("created_at").toString(),"yyyy-MM-ddTHH:mm:ssZ");
	time = time.addSecs(60*60);
	uint64_t uptime = time.secsTo(QDateTime::currentDateTime());
	int viewers = obj.value("stream").toObject().value("viewers").toInt();
	lastUptime = uptime;
	lastViewers = viewers;
	qDebug() << "viewers" << viewers << recordViewers << viewerAlertIssued;
	if (viewers > recordViewers){
		recordViewers = lastViewers;
		//if (!viewerAlertIssued)
		emit sendMessageToChannel(channel,QString("We currently have %1 viewers. This is a new Record!").arg(viewers));
		viewerAlertIssued = true;
	}
	qDebug() << "uptime" << uptime << recordUptime << uptimeAlertIssued;
	if (uptime > recordUptime){
		recordUptime = lastUptime;
		if (!uptimeAlertIssued)
			emit sendMessageToChannel(channel,QString("We are currently live for %1 minutes. This is a new Record!").arg(uptime/60));
		uptimeAlertIssued = true;
	}
}
