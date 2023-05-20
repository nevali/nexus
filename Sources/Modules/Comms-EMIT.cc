#include "Comms.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Comms;

bool
EMIT::execute(ExecutionContext *ctx)
{
	if(argc() != 2)
	{
		ctx->who->send("Usage: @EMIT TEXT\n");
		return false;
	}
	ctx->who->emitf("%s\n", argv(1));
	return true;
}
