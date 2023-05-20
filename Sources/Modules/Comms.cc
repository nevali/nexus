/* Comms module - provides basic communications primitives */

#include <cstdio>

#include "WARP/Flux/Diagnostics.hh"

#include "Comms.hh"

#include "Nexus/Universe.hh"

using namespace Nexus;
using namespace Nexus::Modules::Comms;
using namespace WARP::Flux::Diagnostics;

PROVIDE_MODULE(Comms);

CommandEntry Comms::_commands[] = {
	{ "ECHO", CommandEntry::NONE, Thing::BUILDER, ECHO::construct, "Display a message" },
	{ "EMIT", CommandEntry::NONE, Thing::BUILDER, EMIT::construct, "Broadcast a message to your location" },
	{ NULL, CommandEntry::NONE, 0, NULL, NULL }
};

Comms::Comms(Universe *universe, Module *module):
	ModuleImplementation(universe, module)
{
	::debugf("Commms::%s: module initialised\n", __FUNCTION__);
	addBuiltinCommands(_commands);
}

Comms::~Comms()
{
	removeBuiltinCommands(_commands);
	::debugf("Comms::%s: module deinitialised\n", __FUNCTION__);
}
