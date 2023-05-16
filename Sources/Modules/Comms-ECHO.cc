#include "Comms.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Comms;

bool
ECHO::execute(Actor *who)
{
	if(argc() != 2)
	{
		who->send("Usage: @ECHO TEXT\n");
		return false;
	}
	who->sendf("%s\n", argv(1));
	return true;
}
