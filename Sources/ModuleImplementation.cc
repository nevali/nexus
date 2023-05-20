#include <cstdio>

#include "WARP/Flux/Diagnostics.hh"

#include "Nexus/Universe.hh"
#include "Nexus/Module.hh"
#include "Nexus/Commands.hh"

using namespace Nexus;
using namespace WARP::Flux::Diagnostics;

ModuleImplementation::ModuleImplementation(Universe *universe, Module *module):
	_universe(universe),
	_module(module)
{
	debugf("ModuleImplementation::%s: initialising module '%s' [%s] from %s\n", __FUNCTION__, name(), canonicalName(), path());
}

ModuleImplementation::~ModuleImplementation()
{
	debugf("ModuleImplementation::%s: de-initialising %s\n", __FUNCTION__, canonicalName());
}

/* this should not be overidden */
const char *
ModuleImplementation::name(void) const
{
	if(_module)
	{
		return _module->name();
	}
	return NULL;
}

/* this should not be overidden */
const char *
ModuleImplementation::canonicalName(void) const
{
	if(_module)
	{
		return _module->canonicalName();
	}
	return NULL;
}

/* this should not be overidden */
const char *
ModuleImplementation::path(void) const
{
	if(_module)
	{
		return _module->path();
	}
	return NULL;
}

bool
ModuleImplementation::addBuiltinCommands(CommandEntry *entries)
{
	Parser *parser;

	parser = _universe->builtinsParser();
	bool r = false;

	if(parser)
	{
		r = parser->add(entries);
		parser->release();
	}
	return r;
}

bool
ModuleImplementation::removeBuiltinCommands(CommandEntry *entries)
{
	Parser *parser;
	
	parser = _universe->builtinsParser();
	bool r = false;

	if(parser)
	{
		r = parser->remove(entries);
		parser->release();
	}
	return r;
}
