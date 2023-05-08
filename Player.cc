#include <cstdio>
#include <cstring>
#include <cctype>

#include "Nexus/Player.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;

void
Player::send(const char *str)
{
	::fputs(str, stdout);
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
}
