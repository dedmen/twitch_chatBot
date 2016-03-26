#ifndef EVENTSCHEDULER_H
#define EVENTSCHEDULER_H

#include "event.h"
#include <QObject>
#include <QVector>
#include <QTimer>
class eventScheduler: public QObject
{
	Q_OBJECT
	public:
		eventScheduler();
		~eventScheduler();
		void registerBrEvent(QString steamId,QString channel);
		void registerStreamStatEvent(QString channel);
		QVector<eventBase*> getEvents();
	private:
		QTimer ticker;
		QVector<eventBase*> events;
	signals:
		void sendMessageToChannel(QString channel,QString message);
};



#endif // EVENTSCHEDULER_H
