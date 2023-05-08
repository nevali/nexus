#include <cstdio>
#include <cstring>

#include <jansson.h>

#include "Nexus/Thing.hh"
#include "Nexus/Database.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;

unsigned
Thing::status(void) const
{
	return _status;
}