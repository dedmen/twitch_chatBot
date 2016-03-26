#ifndef TWITCHHELPER_H
#define TWITCHHELPER_H
#include <QJsonDocument>
#include <QJsonObject>
#include "../LoadoutEditor/MainLibrary/httpdownloader.h"
class twitchHelper
{
	public:
		twitchHelper();
		~twitchHelper();
		static QJsonObject getUserChannelInfo(QString username);
		static QJsonObject getUserChannelInfo(QStringRef username);
		static QJsonObject getBRData(QString steamId);
};

#endif // TWITCHHELPER_H
