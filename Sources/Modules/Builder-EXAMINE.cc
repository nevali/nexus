#include "Builder.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
EXAMINE::execute(Actor *actor)
{
	Thing *thing;
	bool r;

	if(argc() > 2)
	{
		actor->send("Usage: @EXAMINE [WHAT]\n");
		return false;
	}
	if(argc() < 2)
	{
		actor->examine(actor);
		return true;
	}
	thing = actor->resolveTarget(argv(1));
	if(!thing)
	{
		actor->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	r = true;
	thing->examine(actor);
	thing->release();
	return r;
}
