#include "Builder.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

bool
EXAMINE::execute(ExecutionContext *ctx)
{
	Thing *thing;
	bool r;

	if(argc() > 2)
	{
		ctx->who->send("Usage: @EXAMINE [WHAT]\n");
		return false;
	}
	if(argc() < 2)
	{
		ctx->who->examine(ctx->who);
		return true;
	}
	thing = ctx->who->resolveIdOrBuiltin(argv(1));
	if(!thing)
	{
		ctx->who->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	r = true;
	thing->examine(ctx->who);
	thing->release();
	return r;
}
