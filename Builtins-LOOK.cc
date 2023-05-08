#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"

using namespace Nexus;

bool
Builtins::LOOK::execute(Actor *actor)
{
	Thing *thing;

	if(_argc > 2)
	{
		actor->send("Usage: @LOOK [WHAT]\n");
		return false;
	}
	if(_argc < 2)
	{
		thing = actor->location();
		thing->look(actor);
		thing->release();
		return true;
	}
	thing = actor->resolveTarget(_argv[1]);
	if(!thing)
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[1]);
		return false;
	}
	thing->look(actor);
	thing->release();
	return true;
}
