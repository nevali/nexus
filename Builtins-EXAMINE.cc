#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"

using namespace Nexus;

bool
Builtins::EXAMINE::execute(Actor *actor)
{
	Thing *thing;
	bool r;

	if(_argc > 2)
	{
		actor->send("Usage: @EXAMINE [WHAT]\n");
		return false;
	}
	if(_argc < 2)
	{
		actor->examine(actor);
		return true;
	}
	thing = actor->resolveTarget(_argv[1]);
	if(!thing)
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[1]);
		return false;
	}
	r = true;
	thing->examine(actor);
	thing->release();
	return r;
}
