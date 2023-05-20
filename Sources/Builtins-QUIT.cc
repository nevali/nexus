#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Player.hh"

using namespace Nexus;

bool
Builtins::QUIT::execute(ExecutionContext *ctx)
{
	ctx->who->send("Goodbye!\n");
	if(ctx->who->isPlayer())
	{
		ctx->who->asPlayer()->disconnect();
	}
	return true;
}
