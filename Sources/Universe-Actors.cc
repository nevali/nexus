#include "Nexus/Universe.hh"
#include "Nexus/Actor.hh"

using namespace Nexus;

void
Universe::activated(Actor *actor)
{
	if(_actors)
	{
		_actors->add(actor);
	}
}

void
Universe::deactivated(Actor *actor)
{
	if(_actors)
	{
		_actors->remove(actor);
	}
}

WARP::Flux::TArray<Actor> *
Universe::actors(void)
{
	if(_actors)
	{
		_actors->retain();
		return _actors;
	}
	return NULL;
}