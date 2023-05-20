#include "Builder.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
LIST::execute(ExecutionContext *ctx)
{
	Thing *thing;

	if(argc() > 2)
	{
		ctx->who->send("Usage: @LIST [WHAT]\n");
		return false;
	}
	if(argc() < 2)
	{
		thing = ctx->who->location();
		thing->list(ctx->who);
		thing->release();
		return true;
	}
	thing = ctx->who->resolveIdOrBuiltin(argv(1));
	if(!thing)
	{
		ctx->who->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	thing->list(ctx->who);
	thing->release();
	return true;
}
