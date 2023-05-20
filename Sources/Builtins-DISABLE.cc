#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;

bool
Builtins::DISABLE::execute(ExecutionContext *ctx)
{
	if(argc() != 2)
	{
		ctx->who->send("Usage: @DISABLE MODULE\n");
		return false;
	}
	if(!_universe->disableModule(argv(1)))
	{
		ctx->who->sendf("Module '%s' could not be disabled\n", argv(1));
		return false;
	}
	ctx->who->sendf("Module '%s' disabled\n", argv(1));
	return true;
}
