#ifndef EVENT_H
#define EVENT_H

#include <QObject>
class streamer;


enum eventTypes {
	null = 0,
	BRRankup = 1,
	streamStats = 2
};




class eventBase : public QObject {
	Q_OBJECT
public:
	eventBase(streamer* pStreamer);
	~eventBase();
	streamer* getOwner() { return owner; };
	eventTypes getEventType() { return eventType; }
private:
protected:
	streamer* owner;
	eventTypes eventType;
signals:
	void sendMessageToChannel(streamer* pStreamer, QString message,bool ignoreMute = false);
	public slots:
	virtual void tickEvent() = 0;
};

class event_BRRankup : public eventBase {
	Q_OBJECT
public:
	event_BRRankup(streamer* pStreamer);
	~event_BRRankup();
	QString getChannel() const;
	QString getSteamID() const;

	public slots:
	virtual void tickEvent();
private:
	QString rankupMessage;
	QString rankdownMessage;
	QString steamID;
	QString channel;
	int lastKills;
	int lastLosses;
	int lastRank;
};

class event_streamStats : public eventBase {
	Q_OBJECT
public:
	event_streamStats(streamer* pStreamer);
	~event_streamStats();
	public slots:
	virtual void tickEvent();
private:
	char failcount;
	QString channel;
	int recordViewers;
	int lastViewers;
	uint64_t lastUptime;
	uint64_t recordUptime;
	bool viewerAlertIssued;
	bool uptimeAlertIssued;
};




#endif // EVENT_H
