#include <cstring>

#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"

using namespace Nexus;

bool
Builtins::FLAG::execute(Actor *actor)
{
	Thing *thing;
	const char *flag;
	bool onOff;

	if(_argc < 3 || _argc > 4)
	{
		actor->send("Usage: @FLAG TARGET FLAG [on|off]\n");
		return false;
	}
	flag = _argv[2];
	if(_argc > 3)
	{
		if(!strcasecmp(_argv[3], "on") || !strcasecmp(_argv[3], "true"))
		{
			onOff = true;
		}
		else if(!strcasecmp(_argv[3], "off") || !strcasecmp(_argv[3], "false"))
		{
			onOff = false;
		}
		else
		{
			actor->sendf("Sorry, I don't understand '%s'\n", _argv[3]);
			return false;
		}
	}
	else
	{
		onOff = true;
	}
	if(!(thing = actor->resolveTarget(_argv[1])))
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[1]);
		return false;
	}
	if(thing->flag(actor, flag, onOff))
	{
		thing->release();
		return true;
	}
	actor->sendf("Sorry, I can't set the %s flag of %s (%s)\n", flag, thing->displayName(), thing->ident());
	thing->release();
	return false;
}
