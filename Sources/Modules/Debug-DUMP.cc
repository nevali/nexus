#include "Debug.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Debug;

bool
DUMP::execute(ExecutionContext *ctx)
{
	Thing *thing;

	if(argc() > 2)
	{
		ctx->who->send("Usage: @EXAMINE [WHAT]\n");
		return false;
	}
	if(argc() < 2)
	{
		ctx->who->dump(ctx->who);
		return true;
	}
	thing = ctx->who->resolveIdOrBuiltin(argv(1));
	if(!thing)
	{
		ctx->who->sendf("Sorry, I can't find '%s'\n", argv(1));
		return false;
	}
	thing->dump(ctx->who);
	thing->release();
	return true;
}
