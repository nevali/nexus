#include "Builder.hh"

#include "Nexus/Actor.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
WHO::execute(Actor *actor)
{
	WARP::Flux::TArray<Actor> *actors = _universe->actors();
	size_t c;


	for(c = 0; c < actors->count(); c++)
	{
		actor->sendf("%s (%s) [%s]\n", actor->displayName(), actor->ident(), actor->flagsStr());
	}
	actors->release();
	return true;
}