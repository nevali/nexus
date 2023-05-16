#include "Builder.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
RENAME::execute(Actor *actor)
{
	Thing *thing;
	const char *newName;

	if(argc() < 2 || argc() > 3)
	{
		actor->send("Usage: @RENAME WHAT [NEW-NAME]\n");
		return false;
	}
	if(argc() > 2)
	{
		newName = argv(2);
		if(!newName[0])
		{
			newName = NULL;
		}
	}
	else
	{
		newName = NULL;
	}
	if(!(thing = actor->resolveTarget(argv(1))))
	{
		actor->sendf("Sorry, I can't find '%s'\n", argv(1));
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
