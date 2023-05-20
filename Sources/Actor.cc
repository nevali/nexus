#include <cstdio>
#include <cstring>
#include <cctype>

#include "WARP/Flux/Diagnostics.hh"

#include "Nexus/Actor.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Channel.hh"

using namespace Nexus;
using namespace WARP::Flux::Diagnostics;

/* Activate this actor */
bool
Actor::activate(void)
{
	if(_universe)
	{
		_universe->activated(this);
		activated();
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
		deactivated();
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
		::debugf("Actor::%s: cannot perform commands without a universe\n", __FUNCTION__);
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

bool
Actor::setFlag(const char *flag, bool set)
{
	const char *flags[] = { "immortal", "invulnerable", "builder",
		"omnipotent", "xray", "debugger", "auditor", "dba", "engineer",
		NULL };
	size_t c;

	for(c = 0; flags[c]; c++)
	{
		if(!strcasecmp(flags[c], flag))
		{
			json_object_set_new(json_object_get(_obj, "flags"), flags[c], json_boolean(set));
			updateFlags();
			return true;
		}
	}
	return Thing::setFlag(flag, set);
}

void
Actor::applyFlag(const char *flag, bool set)
{
	if(!strcmp(flag, "immortal"))
	{
		if(set)
		{
			_flags = _flags | IMMORTAL;
		}
		else
		{
			_flags = _flags & (~IMMORTAL);
		}
		return;
	}
	if(!strcmp(flag, "invulnerable"))
	{
		if(set)
		{
			_flags = _flags | INVULNERABLE;
		}
		else
		{
			_flags = _flags & (~INVULNERABLE);
		}
		return;
	}
	if(!strcmp(flag, "builder"))
	{
		if(set)
		{
			_flags = _flags | BUILDER;
		}
		else
		{
			_flags = _flags & (~BUILDER);
		}
		return;
	}
	if(!strcmp(flag, "omnipotent"))
	{
		if(set)
		{
			_flags = _flags | OMNIPOTENT;
		}
		else
		{
			_flags = _flags & (~OMNIPOTENT);
		}
		return;
	}
	if(!strcmp(flag, "xray"))
	{
		if(set)
		{
			_flags = _flags | XRAY;
		}
		else
		{
			_flags = _flags & (~XRAY);
		}
		return;
	}
	if(!strcmp(flag, "debugger"))
	{
		if(set)
		{
			_flags = _flags | DEBUGGER;
		}
		else
		{
			_flags = _flags & (~DEBUGGER);
		}
		return;
	}
	if(!strcmp(flag, "auditor"))
	{
		if(set)
		{
			_flags = _flags | AUDITOR;
		}
		else
		{
			_flags = _flags & (~AUDITOR);
		}
		return;
	}
	if(!strcmp(flag, "dba"))
	{
		if(set)
		{
			_flags = _flags | DBA;
		}
		else
		{
			_flags = _flags & (~DBA);
		}
		return;
	}
	if(!strcmp(flag, "engineer"))
	{
		if(set)
		{
			_flags = _flags | ENGINEER;
		}
		else
		{
			_flags = _flags & (~ENGINEER);
		}
		return;
	}

	Thing::applyFlag(flag, set);
}

/* Default behaviours when an actor is activated or deactivated */

void
Actor::activated(void)
{
	connectChannels();
}

void
Actor::deactivated(void)
{
	disconnectChannels();
}

/* connect to all saved channels */
void
Actor::connectChannels(void)
{
	if(!_universe)
	{
		return;
	}
}

/* disconnect from all channels */
void
Actor::disconnectChannels(void)
{
	if(!_universe)
	{
		return;
	}
}

bool
Actor::connectToChannel(Channel *chan)
{
	return chan->connect(this);
}

bool
Actor::disconnectFromChannel(Channel *chan)
{
	return chan->disconnect(this);
}
