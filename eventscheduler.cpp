#include "eventscheduler.h"
#include <QDebug>
eventScheduler::eventScheduler() {
	ticker.setInterval(10000);
	ticker.start();
}

eventScheduler::~eventScheduler() {

}

void eventScheduler::registerBrEvent(streamer* pStreamer) {
	qDebug() << __FUNCTION__;
	event_BRRankup* ev = new event_BRRankup(pStreamer);
	connect(ev, &event_BRRankup::sendMessageToChannel, this, &eventScheduler::sendMessageToChannel);
	connect(&this->ticker, &QTimer::timeout, ev, &event_BRRankup::tickEvent);
	pStreamer->addEvent(ev);
	events.append(ev);
}

void eventScheduler::registerStreamStatEvent(streamer* pStreamer) {
	qDebug() << __FUNCTION__;
	event_streamStats* ev = new event_streamStats(pStreamer);
	connect(ev, &event_streamStats::sendMessageToChannel, this, &eventScheduler::sendMessageToChannel);
	connect(&this->ticker, &QTimer::timeout, ev, &event_streamStats::tickEvent);
	pStreamer->addEvent(ev);
	events.append(ev);
}

bool eventScheduler::streamerHasEvent(streamer* pStreamer, eventTypes eventType) {
	for (eventBase* pEvent : events) {
		if (pEvent->getOwner() != pStreamer)
			continue;

		switch (eventType) {
			case BRRankup:
				if (dynamic_cast<event_BRRankup*>(pEvent))
					return true;
				break;
			case streamStats:
				if (dynamic_cast<event_streamStats*>(pEvent))
					return true;
				break;
		}
	}
	return false;
}

void eventScheduler::unregisterEvent(eventBase* pEvent) { //#TODO nullptr check
	qDebug() << __FUNCTION__ << pEvent->getEventType();
	events.removeAll(pEvent);
	pEvent->getOwner()->removeEvent(pEvent);
}

QVector<eventBase*> eventScheduler::getEvents() {
	return events;
}
