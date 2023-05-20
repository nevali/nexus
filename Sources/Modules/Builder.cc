/* Builder module - provides basic building commands */

#include <cstdio>

#include "WARP/Flux/Diagnostics.hh"

#include "Builder.hh"

#include "Nexus/Universe.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;
using namespace WARP::Flux::Diagnostics;

PROVIDE_MODULE(Builder);

CommandEntry Builder::_commands[] = {
	{ "CREATE", CommandEntry::NONE, Thing::BUILDER, CREATE::construct, "Create a new object" },
	{ "DESTROY", CommandEntry::NONE, Thing::BUILDER, DESTROY::construct, "Destroy an object" },
	{ "EXAMINE", CommandEntry::NONE, Thing::BUILDER, EXAMINE::construct, "Show detailed information about an object" },
	{ "FLAG", CommandEntry::NONE, Thing::BUILDER, FLAG::construct, "Change an object's flags" },
	{ "GO", CommandEntry::NONE, Thing::OMNIPOTENT, GO::construct, "Teleport yourself to a location" },
	{ "LIST", CommandEntry::NONE, 0, LIST::construct, "List the contents of an object" },
	{ "LOOK", CommandEntry::NONE, 0, LOOK::construct, "Display the description of an object" },
	{ "RENAME", CommandEntry::NONE, Thing::BUILDER, RENAME::construct, "Change an object's name" },
	{ "SET", CommandEntry::NONE, Thing::BUILDER, SET::construct, "Change an object's properties" },
	{ "TELEPORT", CommandEntry::NONE, Thing::OMNIPOTENT, TELEPORT::construct, "Move an object to a location" },
	{ "WHO", CommandEntry::NONE, Thing::BUILDER|Thing::DEBUGGER, WHO::construct, "List active actors, players, and robots" },
	{ NULL, CommandEntry::NONE, 0, NULL, NULL }
};

Builder::Builder(Universe *universe, Module *module):
	ModuleImplementation(universe, module)
{
	::debugf("Builder::%s: module initialised\n", __FUNCTION__);
	addBuiltinCommands(_commands);
}

Builder::~Builder()
{
	removeBuiltinCommands(_commands);
	::debugf("Builder::%s: module deinitialised\n", __FUNCTION__);
}
