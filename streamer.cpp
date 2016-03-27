#include "streamer.h"
#include <controller.h>
#include <database.h>
streamer::streamer(QObject *parent, controller* pControl, quint32 streamerId, QString channelId) : QObject(parent), control(pControl), streamerId(streamerId),
channelId(channelId) {
	muted = false;
}

streamer::~streamer() {

}

QString streamer::getConfigValue(QString key) {
	return config.value(key);
}

void streamer::setConfigValue(QString key, QString value, bool pushToDatabase) {
	config.insert(key, value);
	if (pushToDatabase)
		control->pDb.setStreamerValue(this, key, value);
}

void streamer::checkMuted() {
	muted = getConfigValue("muted").contains("1");
}

