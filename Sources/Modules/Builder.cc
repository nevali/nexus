/* Builder module - provides basic building commands */

#include <cstdio>

#include "Builder.hh"

#include "Nexus/Universe.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

PROVIDE_MODULE(Builder);

CommandEntry Builder::_commands[] = {
	{ "CREATE", CommandEntry::NONE, CREATE::construct, "Create a new object" },
	{ "DESTROY", CommandEntry::NONE, DESTROY::construct, "Destroy an object" },
	{ "EXAMINE", CommandEntry::NONE, EXAMINE::construct, "Show detailed information about an object" },
	{ "FLAG", CommandEntry::NONE, FLAG::construct, "Change an object's flags" },
	{ "GO", CommandEntry::NONE, GO::construct, "Teleport yourself to a location" },
	{ "LIST", CommandEntry::NONE, LIST::construct, "List the contents of an object" },
	{ "LOOK", CommandEntry::NONE, LOOK::construct, "Display the description of an object" },
	{ "RENAME", CommandEntry::NONE, RENAME::construct, "Change an object's name" },
	{ "SET", CommandEntry::NONE, SET::construct, "Change an object's properties" },
	{ "TELEPORT", CommandEntry::NONE, TELEPORT::construct, "Move an object to a location" },
	{ "WHO", CommandEntry::NONE, WHO::construct, "List active actors, players, and robots" },
	{ NULL, CommandEntry::NONE, NULL, NULL }
};

Builder::Builder(Universe *universe, Module *module):
	ModuleImplementation(universe, module)
{
	fprintf(stderr, "Builder::%s: module initialised\n", __FUNCTION__);
	addBuiltinCommands(_commands);
}

Builder::~Builder()
{
	removeBuiltinCommands(_commands);
	fprintf(stderr, "Builder::%s: module deinitialised\n", __FUNCTION__);
}
