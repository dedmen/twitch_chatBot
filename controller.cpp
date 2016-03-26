#include "controller.h"
#include "database.h"
#include "streamer.h"
controller::controller()
{
	initIRC();
}

controller::~controller()
{

}

void controller::initIRC()
{
	pIrc.init("irc.twitch.tv",6667);
	pIrc.run();
}

void controller::initDB()
{
	pDb.init();
}

void controller::refreshStreamerList()
{
	for (streamerListData streamerData : pDb.getAllStreamers()){
		if (!haveStreamer(streamerData.channelId)){
			streamer* pStreamer = new streamer(this,this,streamerData.index,streamerData.channelId);
			streamerList.append(pStreamer);
			pIrc.addChannel(pStreamer->channelId);
		}
	}

//#TODO get all Events



}

bool controller::haveStreamer(QString channelId)
{
	for (streamer* pStreamer : streamerList)
		if (pStreamer->channelId.compare(channelId) == 0)
			return true;
	return false;
}

void controller::refreshStreamerConfig(streamer* pStreamer)
{
	auto data = pDb.getStreamerConfig(pStreamer);
	for (streamerConfigData keyValuePair : data)
		pStreamer->setConfigValue(keyValuePair.key,keyValuePair.value,false);
}

void controller::ircMessageIn(QString channel, QString user, QString message)
{
    
}

