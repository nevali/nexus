/* Debug module - provides some simple debugging and diagnostic facilities */

#include <cstdio>

#include "Debug.hh"

#include "Nexus/Universe.hh"

using namespace Nexus;
using namespace Nexus::Modules::Debug;

PROVIDE_MODULE(Debug);

CommandEntry Debug::_commands[] = {
	{ "DUMP", CommandEntry::NONE, DUMP::construct, "Dump a JSON representation of an object" },
	{ NULL, CommandEntry::NONE, NULL, NULL }
};

Debug::Debug(Universe *universe, Module *module):
	ModuleImplementation(universe, module)
{
	fprintf(stderr, "Commms::%s: module initialised\n", __FUNCTION__);
	addBuiltinCommands(_commands);
}

Debug::~Debug()
{
	removeBuiltinCommands(_commands);
	fprintf(stderr, "Debug::%s: module deinitialised\n", __FUNCTION__);
}
