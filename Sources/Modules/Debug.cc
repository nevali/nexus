/* Debug module - provides some simple debugging and diagnostic facilities */

#include <cstdio>
#include "WARP/Flux/Diagnostics.hh"

#include "Debug.hh"

#include "Nexus/Universe.hh"

using namespace Nexus;
using namespace Nexus::Modules::Debug;
using namespace WARP::Flux::Diagnostics;

PROVIDE_MODULE(Debug);

CommandEntry Debug::_commands[] = {
	{ "DUMP", CommandEntry::NONE, Thing::ENGINEER|Thing::DBA|Thing::DEBUGGER, DUMP::construct, "Dump a JSON representation of an object" },
	{ NULL, CommandEntry::NONE, 0, NULL, NULL }
};

Debug::Debug(Universe *universe, Module *module):
	ModuleImplementation(universe, module)
{
	debugf("Debug::%s: module initialised\n", __FUNCTION__);
	addBuiltinCommands(_commands);
}

Debug::~Debug()
{
	removeBuiltinCommands(_commands);
	debugf("Debug::%s: module deinitialised\n", __FUNCTION__);
}
