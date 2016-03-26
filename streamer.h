#ifndef STREAMER_H
#define STREAMER_H

#include <QObject>
#include "event.h"
class streamer : public QObject
{
		Q_OBJECT
	public:
		explicit streamer(QObject *parent = 0);
		~streamer();
	private:
		QString channelId;
		quint32 streamerId;
		QVector<eventBase*> events;
	signals:

	public slots:
};

#endif // STREAMER_H
