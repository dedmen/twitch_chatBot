#include "streamer.h"
#include <controller.h>
#include <database.h>
streamer::streamer(QObject *parent, controller* pControl, quint32 streamerId, QString channelId) : QObject(parent), channelId(channelId), control(pControl),
streamerId(streamerId) {
	muted = false;
}

streamer::~streamer() {
	for (eventBase* pEvent : events)
		control->pEventScheduler.unregisterEvent(pEvent);
	control->pIrc.removeChannel(channelId);
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

