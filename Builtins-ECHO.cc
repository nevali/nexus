#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"

using namespace Nexus;

bool
Builtins::ECHO::execute(Actor *who)
{
	if(_argc != 2)
	{
		who->send("Usage: @ECHO TEXT\n");
		return false;
	}
	who->sendf("%s\n", _argv[1]);
	return true;
}
