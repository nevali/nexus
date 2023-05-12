#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"

using namespace Nexus;

bool
Builtins::EMIT::execute(Actor *who)
{
	if(_argc != 2)
	{
		who->send("Usage: @EMIT TEXT\n");
		return false;
	}
	who->emitf("%s\n", _argv[1]);
	return true;
}
