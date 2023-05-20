#include "Builder.hh"

#include "Nexus/Actor.hh"
#include "Nexus/Portal.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
TELEPORT::execute(Actor *actor)
{
	Thing *thing, *dest;
	bool r;

	if(argc() != 3)
	{
		actor->send("Usage: @TELEPORT WHAT WHERE\n");
		return false;
	}
	if(!(thing = actor->resolveIdOrBuiltin(argv(1))))
	{
		actor->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	if(!(dest = actor->resolveIdOrBuiltin(argv(2))))
	{
		actor->sendf("Sorry, I can't find '%s'\n", argv(2));
		thing->release();
		return false;
	}
	r = false;
	if(dest->isContainer())
	{
		Container * container = dest->asContainer();
		dest = NULL;
		r = thing->teleport(actor, container);
		container->release();
	}
	else if(dest->isPortal())
	{
		Portal *portal = dest->asPortal();
		dest = NULL;
		actor->sendf("Sorry, I don't know how to @TELEPORT to Portals yet\n");
		portal->release();
	}
	else
	{
		actor->sendf("%s (%s) is not a container or portal\n", dest->displayName(), dest->ident());
		dest->release();
	}
	thing->release();
	return r;
}
