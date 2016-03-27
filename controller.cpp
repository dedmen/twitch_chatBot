#include "controller.h"
#include "database.h"
#include "streamer.h"
#include "twitchhelper.h"

controller::controller() {
	initDB();
	initIRC();
	refreshStreamerList();
}

controller::~controller() {

}

void controller::initIRC() {
	pIrc.init("irc.twitch.tv", 6667);
	pIrc.run();
	connect(&pEventScheduler, &eventScheduler::sendMessageToChannel, &pIrc, &irc::sendMessageToChannel);
	connect(&pIrc, &irc::ircMessageIn, this, &controller::ircMessageIn);
}

void controller::initDB() {
	pDb.init();
}

void controller::refreshStreamerList() {
	//#TODOlowPrio also remove disabled streamers
	QStringList allowedStreamers;
	for (streamerListData streamerData : pDb.getAllStreamers()) {
		allowedStreamers.append(streamerData.channelId);
		if (!haveStreamer(streamerData.channelId)) {
			qDebug() << "addStreamer" << streamerData.channelId;
			streamer* pStreamer = new streamer(this, this, streamerData.index, streamerData.channelId);
			streamerList.append(pStreamer);
			pIrc.addChannel(pStreamer->channelId);
			refreshStreamerConfig(pStreamer);
			refreshStreamerEvents(pStreamer);
		}
	}
	for (streamer* pStreamer : streamerList) {
		if (!allowedStreamers.contains(pStreamer->channelId)) {
			streamerList.removeAll(pStreamer);
			delete pStreamer;  //also removes his Events
		}
	}
}

bool controller::haveStreamer(QString channelId) {
	for (streamer* pStreamer : streamerList)
		if (pStreamer->channelId.compare(channelId) == 0)
			return true;
	return false;
}

void controller::refreshStreamerConfig(streamer* pStreamer) {
	auto data = pDb.getStreamerConfig(pStreamer);
	for (streamerConfigData keyValuePair : data)
		pStreamer->setConfigValue(keyValuePair.key, keyValuePair.value, false);
	pStreamer->checkMuted();
}

void controller::refreshStreamerEvents(streamer* pStreamer) {
	auto data = pDb.getStreamerEvents(pStreamer);
	for (auto event : data) {
		if (pEventScheduler.streamerHasEvent(pStreamer, (eventTypes) event))
			continue;
		switch ((eventTypes) event) {
			case BRRankup:
				pEventScheduler.registerBrEvent(pStreamer);
				break;
			case streamStats:
				pEventScheduler.registerStreamStatEvent(pStreamer);
				break;
			case null: break;
			default: break;
		}
	}
	for (eventBase* pEvent : pStreamer->getEvents()) {
		if (!data.contains(pEvent->getEventType()))
			pEventScheduler.unregisterEvent(pEvent);
	}
}

streamer* controller::getStreamerByChannel(QString channelId) {
	for (streamer* pStreamer : streamerList)
		if (pStreamer->channelId.compare(channelId) == 0)
			return pStreamer;
	return NULL;
}

void controller::ircMessageIn(QString channel, QString user, QString message) {
	streamer* pStreamer = getStreamerByChannel(channel);

	if (message.contains("dedmen", Qt::CaseInsensitive) && !user.compare("maugrim8888", Qt::CaseInsensitive))
		pIrc.sendMessageToChannel(pStreamer, "Hallo Maugrim!\r\n", true);
	if (message.contains("!viewers")) {
		QJsonObject obj = twitchHelper::getUserChannelInfo(channel);
		//qDebug() << obj.value("stream").toObject().value("viewers").toInt();
		pIrc.sendMessageToChannel(pStreamer, "We currently have " + QString::number(obj.value("stream").toObject().value("viewers").toInt()) + " viewers\r\n");
	}
	if (message.contains("!live") || message.contains("!uptime")) {
		QJsonObject obj = twitchHelper::getUserChannelInfo(channel);
		//qDebug() << obj.value("stream").toObject().value("created_at").toString();
		QDateTime time = QDateTime::fromString(obj.value("stream").toObject().value("created_at").toString(), "yyyy-MM-ddTHH:mm:ssZ");
		time = time.addSecs(60 * 60);
		pIrc.sendMessageToChannel(pStreamer, "We are live for " + QString::number(time.secsTo(QDateTime::currentDateTime()) / 60) + " minutes\r\n");
	}
	if (message.contains("!BR", Qt::CaseInsensitive) || message.contains("!rank", Qt::CaseInsensitive)) {
		//streamer* pStreamer = nullptr;
		////should create a QRunnable and process Async
		//for (eventBase* ev : pEventScheduler.getEvents()) {
		//	event_BRRankup* brev = dynamic_cast<event_BRRankup*>(ev);
		//	if (brev == nullptr)
		//		continue;
		//	if (channel.compare(brev->getChannel()) == 0) {
		//		pStreamer = brev->getOwner();
		//		break;
		//	}
		//}
		if (/*pStreamer == nullptr || */pStreamer->getConfigValue("steamId").isNull()) { //#PERF get config value may get called twice
			pIrc.sendMessageToChannel(pStreamer, "What do you want from me? @" + user);
			return;
		}
		QJsonObject obj = twitchHelper::getBRData(pStreamer->getConfigValue("steamId"));
		int kills = obj.value("kills").toInt();
		int losses = obj.value("losses").toInt();
		int rank = obj.value("global_rank").toInt();


		QString brrank;
		if (brrank = pStreamer->getConfigValue("br_customRankMessage"), brrank.isNull())
			brrank = QString("%4 has been awarded with rank %1 for his %2 Kills and %3 Deaths.");
		brrank = brrank.arg(QString::number(rank), QString::number(kills), QString::number(losses), channel);
		pIrc.sendMessageToChannel(pStreamer, brrank);
	}
	if (!user.compare("mohawpsi") || !user.compare("dedmenmiller")) { //#TODOlowPrio replace with Moderator or some registered user or something
		if (!message.compare("!refresh_streamers")) {
			refreshStreamerList();
			pIrc.sendMessageToChannel(pStreamer, "refreshed streamers");
		}
		if (!message.compare("!refresh_events")) {
			refreshStreamerEvents(pStreamer);
			pIrc.sendMessageToChannel(pStreamer, "refreshed events");
		}
		if (!message.compare("!refresh_config")) {
			refreshStreamerConfig(pStreamer);
			pIrc.sendMessageToChannel(pStreamer, "refreshed config");
		}
	}
	QRegularExpression regExpZockst("was (zockst|spielst) du");
	regExpZockst.setPatternOptions(regExpZockst.patternOptions() | QRegularExpression::CaseInsensitiveOption);
	if (regExpZockst.match(message).hasMatch()) {
		pIrc.sendMessageToChannel(pStreamer, "@" + user + " steht oben!", true);
	}
	if (message.left(3).compare("was", Qt::CaseInsensitive) == 0)
		qDebug() << user << message;

}

