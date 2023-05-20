#include "Builder.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
LOOK::execute(ExecutionContext *ctx)
{
	Thing *thing;

	if(argc() > 2)
	{
		ctx->who->send("Usage: @LOOK [WHAT]\n");
		return false;
	}
	if(argc() < 2)
	{
		thing = ctx->who->location();
		thing->look(ctx->who);
		thing->release();
		return true;
	}
	thing = ctx->who->resolveIdOrBuiltin(argv(1));
	if(!thing)
	{
		ctx->who->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	thing->look(ctx->who);
	thing->release();
	return true;
}
