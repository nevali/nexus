#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Player.hh"

using namespace Nexus;

bool
Builtins::QUIT::execute(Actor *actor)
{
	actor->send("Goodbye!\n");
	if(actor->isPlayer())
	{
		actor->asPlayer()->disconnect();
	}
	return true;
}
