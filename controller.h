#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "irc.h"
#include "eventscheduler.h"
#include "streamer.h"
class controller : public QObject
{
		Q_OBJECT
	public:
		controller();
		~controller();
		void initIRC();
		void initDB();
		void refreshStreamerList();
		bool haveStreamer(QString);
		void refreshStreamerConfig(streamer* pStreamer);
		database pDb; //#TODO make private and forward functions
	private:
		irc pIrc;
		eventScheduler pEventScheduler;
		QList<streamer*> streamerList;
	public slots:
		void ircMessageIn(QString channel,QString user, QString message);

};

#endif // CONTROLLER_H