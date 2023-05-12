#include "Nexus/Actor.hh"
#include "Nexus/Commands.hh"

using namespace Nexus;

bool
Builtins::COMMANDS::execute(Actor *actor)
{
	actor->send("=[ Built-in Commands ]=================================================\n");
	for(size_t c = 0 ; c < _parser->commandCount(); c++)
	{
		CommandEntry *entry = _parser->commandAtIndex(c);
		if(!entry || (entry->flags & CommandEntry::HIDDEN))
		{
			continue;
		}
		actor->sendf("  @%-20s  %s\n", entry->name, (entry->desc ? entry->desc : ""));
	}	
	actor->send("=======================================================================\n");
	return true;
}
