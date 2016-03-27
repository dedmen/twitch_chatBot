#ifndef EVENTSCHEDULER_H
#define EVENTSCHEDULER_H

#include "event.h"
#include <QObject>
#include <QVector>
#include <QTimer>
#include "streamer.h"
class eventScheduler : public QObject {
	Q_OBJECT
public:
	eventScheduler();
	~eventScheduler();
	void registerBrEvent(streamer* pStreamer);
	void registerStreamStatEvent(streamer* pStreamer);
	bool streamerHasEvent(streamer* pStreamer, eventTypes eventType);
	void unregisterEvent(eventBase* pEvent);
	QVector<eventBase*> getEvents();
private:
	QTimer ticker;
	QVector<eventBase*> events;
signals:
	void sendMessageToChannel(streamer* pStreamer, QString message, bool ignoreMute = false);
};



#endif // EVENTSCHEDULER_H
