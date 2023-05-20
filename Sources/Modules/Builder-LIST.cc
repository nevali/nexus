#include "Builder.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
LIST::execute(Actor *actor)
{
	Thing *thing;

	if(argc() > 2)
	{
		actor->send("Usage: @LIST [WHAT]\n");
		return false;
	}
	if(argc() < 2)
	{
		thing = actor->location();
		thing->list(actor);
		thing->release();
		return true;
	}
	thing = actor->resolveIdOrBuiltin(argv(1));
	if(!thing)
	{
		actor->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	thing->list(actor);
	thing->release();
	return true;
}
