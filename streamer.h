#ifndef STREAMER_H
#define STREAMER_H

#include <QObject>
#include "event.h"
#include <QMap>
class controller;
class streamer : public QObject {
	Q_OBJECT
public:
	explicit streamer(QObject *parent, controller* pControl, quint32 streamerId, QString channelId);	//#MAYBE push parent to end with default value
	~streamer();
	QString channelId;
	QString getConfigValue(QString key);
	void setConfigValue(QString key, QString value, bool pushToDatabase = true);
	quint32 getStreamerId() const { return streamerId; }
	void addEvent(eventBase* pEvent) { events.append(pEvent); };
	void removeEvent(eventBase* pEvent) { events.removeAll(pEvent); };
	QList<eventBase*> getEvents() { return events; };
	bool muted;
	bool isLive;
	void checkMuted();
private:
	controller* control;
	quint32 streamerId; //cant be 0 without error
	QList<eventBase*> events;
	QMap<QString, QString> config;
signals:

	public slots :
};

#endif // STREAMER_H
