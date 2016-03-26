#include "streamer.h"
#include <controller.h>
#include <database.h>
streamer::streamer(QObject *parent, irc* pControl, quint32 streamerId, QString channelId) : QObject(parent), control(pControl), streamerId(streamerId),
	channelId(channelId)
{

}

streamer::~streamer()
{

}

QString streamer::getConfigValue(QString key)
{
	return config.value(key);
}

void streamer::setConfigValue(QString key, QString value,bool pushToDatabase = true)
{
	config.insert(key,value);
	if (pushToDatabase)
		control->getPDb().setStreamerValue(this, key, value);
}

