#include "Comms.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Comms;

bool
ECHO::execute(ExecutionContext *ctx)
{
	if(argc() != 2)
	{
		ctx->who->send("Usage: @ECHO TEXT\n");
		return false;
	}
	ctx->who->sendf("%s\n", argv(1));
	return true;
}
