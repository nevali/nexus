#include "Builder.hh"

#include "Nexus/Actor.hh"
#include "Nexus/Portal.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
GO::execute(ExecutionContext *ctx)
{
	Thing *dest;
	bool r;

	r = false;
	if(argc() != 2)
	{
		ctx->who->send("Usage: @GO WHERE\n");
		return false;
	}
	/* XXX resolveDestination */
	if(!(dest = ctx->who->resolveIdOrBuiltin(argv(1))))
	{
		ctx->who->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	r = false;
	if(dest->isContainer())
	{
		Container * container = dest->asContainer();
		dest = NULL;
		r = ctx->who->teleport(ctx->who, container);
		container->release();
	}
	else if(dest->isPortal())
	{
		Portal *portal = dest->asPortal();
		dest = NULL;
		ctx->who->sendf("Sorry, I don't know how to @GO to Portals yet\n");
		portal->release();
	}
	else
	{
		ctx->who->sendf("%s (%s) is not a container or portal\n", dest->displayName(), dest->ident());
		dest->release();
	}
	return r;
}
