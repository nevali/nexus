#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"

using namespace Nexus;

bool
Builtins::DESTROY::execute(Actor *actor)
{
	Thing *thing;
	bool r;

	if(_argc != 2)
	{
		actor->send("Usage: @DESTROY WHAT\n");
		return false;
	}
	thing = actor->resolveTarget(_argv[1]);
	if(!thing)
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[1]);
		return false;
	}
	r = true;
	thing->destroy(actor);
	thing->release();
	return r;
}
