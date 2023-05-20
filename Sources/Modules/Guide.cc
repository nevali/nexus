/* Guide module - provides the Operator's Guide via @HELP */

#include <cstdio>

#include "Guide.hh"

#include "Nexus/Universe.hh"

using namespace Nexus;
using namespace Nexus::Modules::Guide;

PROVIDE_MODULE(Guide);

CommandEntry Guide::_commands[] = {
	{ "HELP", CommandEntry::NONE, 0, HELP::construct, "Display the Operator's Guide" },
	{ NULL, CommandEntry::NONE, 0, NULL, NULL }
};

Guide::Guide(Universe *universe, Module *module):
	ModuleImplementation(universe, module)
{
	fprintf(stderr, "Commms::%s: module initialised\n", __FUNCTION__);
	addBuiltinCommands(_commands);
}

Guide::~Guide()
{
	removeBuiltinCommands(_commands);
	fprintf(stderr, "Guide::%s: module deinitialised\n", __FUNCTION__);
}
