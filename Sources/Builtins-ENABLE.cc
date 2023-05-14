#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;

bool
Builtins::ENABLE::execute(Actor *who)
{
	if(_argc != 2)
	{
		who->send("Usage: @ENABLE MODULE\n");
		return false;
	}
	if(!_universe->enableModule(_argv[1]))
	{
		who->sendf("Module '%s' could not be enabled\n", _argv[1]);
		return false;
	}
	who->sendf("Module '%s' enabled\n", _argv[1]);
	return true;
}
