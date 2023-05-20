#include "Builder.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
DESTROY::execute(Actor *actor)
{
	Thing *thing;
	bool r;

	if(argc() != 2)
	{
		actor->send("Usage: @DESTROY WHAT\n");
		return false;
	}
	thing = actor->resolveIdOrBuiltin(argv(1));
	if(!thing)
	{
		actor->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	r = true;
	thing->destroy(actor);
	thing->release();
	return r;
}
