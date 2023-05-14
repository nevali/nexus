/* Comms module - provides basic communications primitives */

#include <cstdio>

#include "Comms.hh"

#include "Nexus/Universe.hh"

using namespace Nexus;
using namespace Nexus::Modules::Comms;

PROVIDE_MODULE(Comms);

CommandEntry Comms::_commands[] = {
	{ "ECHO", CommandEntry::NONE, ECHO::construct, "Display a message" },
	{ "EMIT", CommandEntry::NONE, EMIT::construct, "Broadcast a message to your location" },
	{ NULL, CommandEntry::NONE, NULL, NULL }
};

Comms::Comms(Universe *universe, Module *module):
	ModuleImplementation(universe, module)
{
	fprintf(stderr, "Commms::%s: module initialised\n", __FUNCTION__);
	addBuiltinCommands(_commands);
}

Comms::~Comms()
{
	removeBuiltinCommands(_commands);
	fprintf(stderr, "Comms::%s: module deinitialised\n", __FUNCTION__);
}
