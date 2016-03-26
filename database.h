#ifndef DATABASE_H
#define DATABASE_H


#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlResult>
#include <QtSql/QSqlError>
#include <QString>
class database : public QObject
{
		Q_OBJECT
	public:
		explicit database(QObject *parent = 0);
		~database();
		QSqlDatabase db;
		void setStreamerValue(QString channelId, QString key, QString value);
		QString getStreamerValue(QString channelId,QString key);
	public slots:
};

#endif // DATABASE_H
