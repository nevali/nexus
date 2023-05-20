#include <cstdio>
#include <cstring>
#include <cctype>

#include "WARP/Flux/Diagnostics.hh"

#include "Nexus/Player.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Channel.hh"

using namespace Nexus;
using namespace WARP::Flux::Diagnostics;

Player::~Player()
{
	disconnect();
}

void
Player::send(const char *str)
{
	::fputs(str, stdout);
}

void
Player::flush(void)
{
	fflush(stdout);
}

void
Player::connect(void)
{
	Container *loc;
	const char *msg;

	if(_universe)
	{
		if((msg = _universe->messageNamed("connect")))
		{
			send(msg);
		}
	}
	sendf("\nGreetings, %s!\n\n", displayName());
	activate();
	_universe->auditf("[%s (%s) has connected]\n", displayName(), ident());
	_connected = true;
	loc = location();
	if(!loc)
	{
		::debugf("Player::%s: ERROR: location() returned NULL\n", __FUNCTION__);
		disconnect();
		return;
	}
	setLocation(loc);
	loc->look(this);
	loc->release();
	sendPrompt();
}

void
Player::connect(WARP::Flux::Buffer *inputBuffer)
{
	if(_connected)
	{
		disconnect();
	}
	::debugf("Player::%s: retaining Buffer\n", __FUNCTION__);
	inputBuffer->retain();
	inputBuffer->setBufferDelegate(this);
	_inputBuffer = inputBuffer;
	connect();
}

void
Player::connect(WARP::Flux::Channel *channel)
{
	if(_connected)
	{
		disconnect();
	}
	::debugf("Player::%s: creating Buffer\n", __FUNCTION__);
	_inputBuffer = new WARP::Flux::Buffer(this);
	::debugf("Player::%s: retaining Flux::Channel\n", __FUNCTION__);
	channel->retain();
	channel->setChannelDelegate(_inputBuffer);
	_channel = channel;
	connect();
}

void
Player::disconnect(void)
{
	Container *loc;

	deactivate();
	_connected = false;
	loc = location();
	if(loc)
	{
		loc->remove(this);
		loc->release();
	}
	if(_channel)
	{
		::debugf("Player::%s: releasing Flux::Channel\n", __FUNCTION__);
		_channel->clearChannelDelegate(_inputBuffer);
		_channel->release();
		_channel = NULL;
	}
	if(_inputBuffer)
	{
		::debugf("Player::%s: releasing Buffer\n", __FUNCTION__);
		_inputBuffer->clearBufferDelegate(this);
		_inputBuffer->release();
		_inputBuffer = NULL;
	}
	::debugf("Player::%s: disconnected\n", __FUNCTION__);
}

void
Player::bufferFilled(WARP::Flux::Object *sender, WARP::Flux::Buffer *buffer, char *base, size_t *nbytes)
{
	(void) sender;
	(void) buffer;
	(void) base;
	(void) nbytes;
	
//	::debugf("Player::%s() %ld bytes available in buffer\n", __FUNCTION__, *nbytes);
	/* assume for now there's an EOL */
	perform(base);
	if(_universe)
	{
		_universe->sync();
	}
	sendPrompt();
}

void
Player::sendPrompt(void)
{
	sendf("%s> ", _universe->name());
	flush();
}

void
Player::sourceClosed(WARP::Flux::Object *sender, WARP::Flux::Object *source)
{
	(void) sender;
	(void) source;

	::debugf("Player::%s: source was closed\n", __FUNCTION__);
	_universe->auditf("[%s (%s) has disconnected]\n", displayName(), ident());
	disconnect();
}

void
Player::connectChannels(void)
{
	Channel *chan;

	Actor::connectChannels();
	if((chan = _universe->globalChannel()))
	{
		connectToChannel(chan);
		chan->release();
	}
	if(_flags & (DEBUGGER|ENGINEER))
	{
		if((chan = _universe->debugChannel()))
		{
			connectToChannel(chan);
			chan->release();
		}
	}
	if(_flags & AUDITOR)
	{
		if((chan = _universe->auditChannel()))
		{
			connectToChannel(chan);
			chan->release();
		}
	}
}
