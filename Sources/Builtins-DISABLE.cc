#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;

bool
Builtins::DISABLE::execute(Actor *who)
{
	if(_argc != 2)
	{
		who->send("Usage: @DISABLE MODULE\n");
		return false;
	}
	if(!_universe->disableModule(_argv[1]))
	{
		who->sendf("Module '%s' could not be disabled\n", _argv[1]);
		return false;
	}
	who->sendf("Module '%s' disabled\n", _argv[1]);
	return true;
}
