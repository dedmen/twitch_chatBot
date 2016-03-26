#ifndef EVENT_H
#define EVENT_H

#include <QObject>

class eventBase: public QObject
{
		Q_OBJECT
	public:
		eventBase();
		~eventBase();

	signals:
		void sendMessageToChannel(QString channel,QString message);
	public slots:
		virtual void tickEvent() = 0;
};

class event_BRRankup : public eventBase
{
		Q_OBJECT
	public:
		event_BRRankup(QString steamId,QString channel);
		~event_BRRankup();
		QString getChannel() const;
		QString getSteamID() const;

	public slots:
		virtual void tickEvent();
	private:
		QString steamID;
		QString channel;
		int lastKills;
		int lastLosses;
		int lastRank;
};

class event_streamStats : public eventBase
{
		Q_OBJECT
	public:
		event_streamStats(QString channel);
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
