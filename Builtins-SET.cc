#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"

using namespace Nexus;

bool
Builtins::SET::execute(Actor *actor)
{
	Thing *thing;
	const char *property, *newValue;

	if(_argc < 3 || _argc > 4)
	{
		actor->send("Usage: @SET TARGET PROPERTY [NEW-VALUE]\n");
		return false;
	}
	property = _argv[2];
	if(_argc > 3)
	{
		newValue = _argv[3];
	}
	else
	{
		newValue = NULL;
	}
	if(!(thing = actor->resolveTarget(_argv[1])))
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[1]);
		return false;
	}
	if(thing->set(actor, property, newValue))
	{
		thing->release();
		return true;
	}
	if(newValue)
	{
		actor->sendf("Sorry, I can't set the %s of %s (%s) to '%s'\n", property, thing->displayName(), thing->ident(), newValue);
	}
	else
	{
		actor->sendf("Sorry, I can't remove the %s of %s (%s)\n", property, thing->displayName(), thing->ident());
	}
	return true;
}
