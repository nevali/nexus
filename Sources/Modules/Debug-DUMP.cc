#include "Debug.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Debug;

bool
DUMP::execute(Actor *actor)
{
	Thing *thing;

	if(argc() > 2)
	{
		actor->send("Usage: @EXAMINE [WHAT]\n");
		return false;
	}
	if(argc() < 2)
	{
		actor->dump(actor);
		return true;
	}
	thing = actor->resolveTarget(argv(1));
	if(!thing)
	{
		actor->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	thing->dump(actor);
	thing->release();
	return true;
}
