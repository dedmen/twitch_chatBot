#include "eventscheduler.h"
#include <QDebug>
eventScheduler::eventScheduler()
{
	ticker.setInterval(10000);
	ticker.start();
}

eventScheduler::~eventScheduler()
{

}

void eventScheduler::registerBrEvent(QString steamId, QString channel)
{
	qDebug() << __FUNCTION__;
	event_BRRankup* ev = new event_BRRankup(steamId,channel);
	connect (ev,&event_BRRankup::sendMessageToChannel,this,&eventScheduler::sendMessageToChannel);
	connect(&this->ticker,&QTimer::timeout,ev,&event_BRRankup::tickEvent);
	events.append(ev);
}

void eventScheduler::registerStreamStatEvent(QString channel)
{
	qDebug() << __FUNCTION__;
	event_streamStats* ev = new event_streamStats(channel);
	connect (ev,&event_streamStats::sendMessageToChannel,this,&eventScheduler::sendMessageToChannel);
	connect(&this->ticker,&QTimer::timeout,ev,&event_streamStats::tickEvent);
	events.append(ev);
}

QVector<eventBase*> eventScheduler::getEvents()
{
	return events;
}
