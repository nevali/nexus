#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Portal.hh"

using namespace Nexus;

bool
Builtins::GO::execute(Actor *actor)
{
	Thing *dest;
	bool r;

	r = false;
	if(_argc != 2)
	{
		actor->send("Usage: @GO WHERE\n");
		return false;
	}
	if(!(dest = actor->resolveTarget(_argv[1])))
	{
		actor->sendf("Sorry, I can't find '%s'\n", _argv[1]);
		return false;
	}
	r = false;
	if(dest->isContainer())
	{
		Container * container = dest->asContainer();
		dest = NULL;
		r = actor->teleport(actor, container);
		container->release();
	}
	else if(dest->isPortal())
	{
		Portal *portal = dest->asPortal();
		dest = NULL;
		actor->sendf("Sorry, I don't know how to @GO to Portals yet\n");
		portal->release();
	}
	else
	{
		actor->sendf("%s (%s) is not a container or portal\n", dest->displayName(), dest->ident());
		dest->release();
	}
	return r;
}
