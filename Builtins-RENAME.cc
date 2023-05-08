#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"

using namespace Nexus;

bool
Builtins::RENAME::execute(Actor *actor)
{
	Thing *thing;
	const char *newName;

	if(_argc < 2 || _argc > 3)
	{
		actor->send("Usage: @RENAME WHAT [NEW-NAME]\n");
		return false;
	}
	if(_argc > 2)
	{
		newName = _argv[2];
		if(!newName[0])
		{
			newName = NULL;
		}
	}
	else
	{
		newName = NULL;
	}
	if(!(thing = actor->resolveTarget(_argv[1])))
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[1]);
		return false;
	}
	if(thing->setName(newName))
	{
		thing->release();
		return true;
	}
	if(newName)
	{
		actor->sendf("Sorry, I can't rename %s (%s) to '%s'\n", thing->displayName(), thing->ident(), newName);
	}
	else
	{
		actor->sendf("Sorry, I can't remove the name of %s (%s)\n", thing->displayName(), thing->ident());
	}
	return true;
}
