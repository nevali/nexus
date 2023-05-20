#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;

bool
Builtins::ENABLE::execute(ExecutionContext *ctx)
{
	if(argc() != 2)
	{
		ctx->who->send("Usage: @ENABLE MODULE\n");
		return false;
	}
	if(!_universe->enableModule(argv(1)))
	{
		ctx->who->sendf("Module '%s' could not be enabled\n", argv(1));
		return false;
	}
	ctx->who->sendf("Module '%s' enabled\n", argv(1));
	return true;
}
