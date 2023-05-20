#include "Builder.hh"

#include "Nexus/Actor.hh"
#include "Nexus/Portal.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
TELEPORT::execute(ExecutionContext *ctx)
{
	Thing *thing, *dest;
	bool r;

	if(argc() != 3)
	{
		ctx->who->send("Usage: @TELEPORT WHAT WHERE\n");
		return false;
	}
	if(!(thing = ctx->who->resolveIdOrBuiltin(argv(1))))
	{
		ctx->who->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	if(!(dest = ctx->who->resolveIdOrBuiltin(argv(2))))
	{
		ctx->who->sendf("Sorry, I can't find '%s'\n", argv(2));
		thing->release();
		return false;
	}
	r = false;
	if(dest->isContainer())
	{
		Container * container = dest->asContainer();
		dest = NULL;
		r = thing->teleport(ctx->who, container);
		container->release();
	}
	else if(dest->isPortal())
	{
		Portal *portal = dest->asPortal();
		dest = NULL;
		ctx->who->sendf("Sorry, I don't know how to @TELEPORT to Portals yet\n");
		portal->release();
	}
	else
	{
		ctx->who->sendf("%s (%s) is not a container or portal\n", dest->displayName(), dest->ident());
		dest->release();
	}
	thing->release();
	return r;
}
