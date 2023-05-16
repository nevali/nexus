#include "Comms.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Comms;

bool
EMIT::execute(Actor *who)
{
	if(argc() != 2)
	{
		who->send("Usage: @EMIT TEXT\n");
		return false;
	}
	who->emitf("%s\n", argv(1));
	return true;
}
