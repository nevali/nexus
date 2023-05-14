/* Builder module - provides basic building commands */

#include <cstdio>

#include "Builder.hh"

#include "Nexus/Universe.hh"

using namespace Nexus;
using namespace Nexus::Modules::Builder;

PROVIDE_MODULE(Builder);

CommandEntry Builder::_commands[] = {
	{ "CREATE", CommandEntry::NONE, CREATE::construct, NULL },
	{ "DESTROY", CommandEntry::NONE, DESTROY::construct, NULL },
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
