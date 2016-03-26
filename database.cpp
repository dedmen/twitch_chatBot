#include "database.h"

#include <QApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "irc.h"
database::database(QObject *parent) : QObject(parent)
{
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(QApplication::applicationDirPath()+"/database.db");
	if (!db.open())
		QMessageBox::warning(0,"KiriLoadoutHelper",QString("Cant OpenDB\n")+db.lastError().text());
	qDebug() << "OpenDB"<<db.lastError().text() << db.drivers();
	QSqlQuery query;
	query.exec("CREATE TABLE IF NOT EXISTS streamerConfig "
			   "(id integer primary key, "
			   "channelId text(256),"
			   "key text(256),"
			   "value text(4096)"//4KB
			   ")");
	qDebug() << "Create streamerConfig" <<query.lastError().text();

}

database::~database()
{

}

void database::setStreamerValue(QString channelId, QString key, QString value)
{

	QSqlQuery query;
	qDebug() << __FUNCTION__ <<  key << value;

	query.exec(QString("SELECT key,value FROM streamerConfig WHERE key == '"+key+"' AND channelId == '"+channelId+"';"));
	query.next();
	if (query.record().value("key").compare(key) != 0 ){
		if (!query.exec(QString("INSERT INTO streamerConfig (channelId,key,value) VALUES ('"+channelId+"','"+key+"','"+value+"');"))){
			qWarning() << "setConfigInsert failed" << query.lastError().text();
		}
		qWarning() << "setConfigInsert" << query.lastError().text();
		return;
	}else {
		if (!query.exec(QString("UPDATE streamerConfig  SET value = '"+value+"' WHERE key = '"+key+"' AND channelId == '"+channelId+"';"))){
			qWarning() << "setConfigUpdate failed" << query.lastError().text();
		}
		qWarning() << "setConfigUpdate" << query.lastError().text();
		return;
	}
}

QString database::getStreamerValue(QString channelId, QString key)
{
	QSqlQuery query;
	if (query.exec(QString("SELECT value FROM streamerConfig WHERE key == '"+key+"' AND channelId == '"+channelId+"';"))){
		while (query.next()){
			qDebug() << query.record() << query.record().value("value").toString();
			return query.record().value("value").toString();
		}
	} else {
		qWarning() << "getConfig" << query.lastError().text();
	}
	return QString();
}


