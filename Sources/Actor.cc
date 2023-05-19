#include <cstdio>
#include <cstring>
#include <cctype>

#include "Nexus/Actor.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Commands.hh"

using namespace Nexus;

/* Activate this actor */
bool
Actor::activate(void)
{
	if(_universe)
	{
		_universe->activated(this);
		return true;
	}
	return false;
}

/* Deactivate this actor */
bool
Actor::deactivate(void)
{
	if(_universe)
	{
		_universe->deactivated(this);
		return true;
	}
	return false;
}

/* Perform a command */
bool
Actor::perform(const char *commandLine)
{
	Parser *parser;
	Command *cmd;

	if(!commandLine)
	{
		return true;
	}
	if(!_universe)
	{
		fprintf(stderr, "Actor::%s: cannot perform commands without a universe\n", __FUNCTION__);
		return false;
	}
	for(; *commandLine && (isblank(*commandLine) || *commandLine == '\r' || *commandLine == '\n'); commandLine++);
	if(!*commandLine)
	{
		return true;
	}
	parser = _universe->parserForCommand(this, commandLine);
	if(parser)
	{
		cmd = parser->parse(this, commandLine);
		parser->release();
		if(cmd)
		{
			bool r;

			r = cmd->execute(this);
			cmd->release();
			return r;
		}
		return false;		
	}
	send("I'm sorry, I don't know how to do that.\n");
	return false;
}
