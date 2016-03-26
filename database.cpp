#include "database.h"

#include <QApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "irc.h"
#include "localdata.h"
database::database(QObject *parent) : QObject(parent)
{
	//db = QSqlDatabase::addDatabase("QSQLITE");
	//db.setDatabaseName(QApplication::applicationDirPath()+"/database.db");
	//if (!db.open())
	//	QMessageBox::warning(0,"KiriLoadoutHelper",QString("Cant OpenDB\n")+db.lastError().text());
	//qDebug() << "OpenDB"<<db.lastError().text() << db.drivers();
	//QSqlQuery query;
	//query.exec("CREATE TABLE IF NOT EXISTS streamerConfig "
	//		   "(id integer primary key, "
	//		   "channelId text(256),"
	//		   "key text(256),"
	//		   "value text(4096)"//4KB
	//		   ")");
	//qDebug() << "Create streamerConfig" <<query.lastError().text();

}

database::~database()
{

}

void database::init()
{

	db = QSqlDatabase::addDatabase("QMYSQL3","streamBot");
	db.setHostName(QStringLiteral("updtDB.dedmen.de"));
	db.setDatabaseName(QStringLiteral("ModpackUpdaterC"));
	db.setUserName(dbUser);
	db.setPassword(dbMUPw);
	bool ok = db.open();
	if (!ok) {
		qCritical() << "DB1 Failed" << db.lastError().text();
		db.setHostName(QStringLiteral("updtDB2.dedmen.de"));
		db.setUserName(dbUser);
		db.setPassword(dbMUPw);
		ok = db.open();
	}
	if (!ok)
		qFatal("no db");
	db.setConnectOptions("MYSQL_OPT_RECONNECT=1;"); //QMYSQLDriver::open: Unknown connect option 'MYSQL_OPT_COMPRESS'
}

void database::setStreamerValue(streamer* pStreamer, QString key, QString value)
{
	QSqlQuery query;
	qDebug() << __FUNCTION__ <<  key << value;
	if (query.exec(QString("CALL proc_setStreamerConfig('"+QString::number(pStreamer->getStreamerId())+"','"+key+"','"+value+"')"))){	//#TODO use .arg()
		return;
	}else {
		qWarning() << __FUNCTION__ << query.lastError().text();
		return;
	}
}

QString database::getStreamerValue(QString channelId, QString key)
{
	//QSqlQuery query;
	//if (query.exec(QString("CALL proc_getStreamerConfigs('"++"');"))){
	//	while (query.next()){
	//		qDebug() << query.record() << query.record().value("value").toString();
	//		return query.record().value("value").toString();
	//	}
	//} else {
	//	qWarning() << "getConfig" << query.lastError().text();
	//}
	return QString();
}

QList<streamerConfigData> database::getStreamerConfig(streamer* pStreamer)
{
	QList<streamerConfigData>  pList;
	QSqlQuery query;
	if (query.exec(QString("CALL proc_getStreamerConfigs('"+QString::number(pStreamer->getStreamerId())+"');"))){
		while (query.next()){
			streamerConfigData pData;
			pData.key = query.record().value("key").toString();
			pData.value = query.record().value("value").toString();
			pList.append(pData);
		}
	} else {
		qWarning() << __FUNCTION__ << query.lastError().text();
	}
	return pList;
}

QList<streamerListData> database::getAllStreamers()
{
	QList<streamerListData> pList;
	QSqlQuery query;
	if (query.exec(QString("CALL proc_getStreamerList();"))){
		while (query.next()){
			streamerListData pData;
			pData.index = query.record().value("index").toUInt();
			pData.channelId = query.record().value("channelId").toString();
			pList.append(pData);
		}
	} else {
		qWarning() << __FUNCTION__ << query.lastError().text();
	}
	return pList;
}


