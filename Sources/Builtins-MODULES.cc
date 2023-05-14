#include "WARP/Flux/Array.hh"

#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Module.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;

bool
Builtins::MODULES::execute(Actor *actor)
{
	WARP::Flux::TArray<Module> *modules;
	Module *mod;
	size_t index;

	actor->send("=[ Active Modules ]====================================================\n");

	modules = _universe->modules();
	for(index = 0; index < modules->count(); index++)
	{
		mod = modules->objectAtIndex(index);
		if(mod)
		{
			actor->sendf("%s\n", mod->name());
			mod->release();
		}
	}
	modules->release();

	actor->send("=======================================================================\n");
	return true;
}
