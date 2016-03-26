#ifndef DATABASE_H
#define DATABASE_H


#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlResult>
#include <QtSql/QSqlError>
#include <QString>

struct streamerListData{
		quint32 index;
		QString channelId;
};
struct streamerConfigData{
	QString key;
	QString value;
};
#include "streamer.h"


class database : public QObject
{
		Q_OBJECT
	public:
		explicit database(QObject *parent = 0);
		~database();
		void init();
		QSqlDatabase db;
		void setStreamerValue(streamer* pStreamer, QString key, QString value);
		[[deprecated]]
		QString getStreamerValue(QString channelId,QString key);
		QList<streamerConfigData> getStreamerConfig(streamer* pStreamer);
		QList<streamerListData> getAllStreamers();
	public slots:
};

#endif // DATABASE_H
