#ifndef EVENTSCHEDULER_H
#define EVENTSCHEDULER_H

#include "event.h"
#include <QObject>
#include <QVector>
#include <QTimer>
#include "streamer.h"
class eventScheduler: public QObject
{
	Q_OBJECT
	public:
		eventScheduler();
		~eventScheduler();
		void registerBrEvent(streamer* pStreamer);
		void registerStreamStatEvent(streamer* pStreamer);
		QVector<eventBase*> getEvents();
	private:
		QTimer ticker;
		QVector<eventBase*> events;
	signals:
		void sendMessageToChannel(QString channel,QString message);
};



#endif // EVENTSCHEDULER_H
