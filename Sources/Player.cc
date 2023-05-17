#include <cstdio>
#include <cstring>
#include <cctype>

#include "Nexus/Player.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;

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
	_connected = true;
	loc = location();
	if(!loc)
	{
		fprintf(stderr, "Player::%s: ERROR: location() returned NULL\n", __FUNCTION__);
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
	fprintf(stderr, "Player::%s: retaining Buffer\n", __FUNCTION__);
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
	fprintf(stderr, "Player::%s: creating Buffer\n", __FUNCTION__);
	_inputBuffer = new WARP::Flux::Buffer(this);
	fprintf(stderr, "Player::%s: retaining Channel\n", __FUNCTION__);
	channel->retain();
	channel->setChannelDelegate(_inputBuffer);
	_channel = channel;
	connect();
}

void
Player::disconnect(void)
{
	Container *loc;

	_connected = false;
	loc = location();
	if(loc)
	{
		loc->remove(this);
		loc->release();
	}
	if(_channel)
	{
		fprintf(stderr, "Player::%s: releasing Channel\n", __FUNCTION__);
		_channel->release();
		_channel = NULL;
	}
	if(_inputBuffer)
	{
		fprintf(stderr, "Player::%s: releasing Buffer\n", __FUNCTION__);
		_inputBuffer->release();
		_inputBuffer = NULL;
	}
	fprintf(stderr, "Player::%s: disconnected\n", __FUNCTION__);
}

void
Player::bufferFilled(WARP::Flux::Object *sender, WARP::Flux::Buffer *buffer, char *base, size_t *nbytes)
{
	(void) sender;
	(void) buffer;
	(void) base;

	fprintf(stderr, "Player::%s() %ld bytes available in buffer\n", __FUNCTION__, *nbytes);
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

	fprintf(stderr, "Player::%s: source was closed\n", __FUNCTION__);
	disconnect();
}
