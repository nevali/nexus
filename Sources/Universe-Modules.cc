#include <cstdio>
#include <cstring>

#include <jansson.h>

#include "Nexus/Universe.hh"
#include "Nexus/Module.hh"

using namespace Nexus;

/* result is retained for you */
Universe::ModuleList *
Universe::modules(void)
{
	if(_modules)
	{
		_modules->retain();
		return _modules;
	}
	return NULL;
}

bool
Universe::activateModules(void)
{
	if(_modules)
	{
		return _modules->activate();
	}
	return false;
}

bool
Universe::enableModule(const char *name)
{
	if(_modules)
	{
		return _modules->enable(name);
	}
	return false;
}

bool
Universe::disableModule(const char *name)
{
	if(_modules)
	{
		return _modules->disable(name);
	}
	return false;
}

Universe::ModuleList::ModuleList(Universe *universe):
	TArray<Module>(),
	_universe(universe),
	_modlist(NULL),
	_activating(false)
{
	_modlist = json_object_get(universe->_meta, "modules");
	if(!_modlist || !json_is_object(_modlist))
	{
		_modlist = json_object();
		json_object_set(universe->_meta, "modules", _modlist);
	}
	else
	{
		json_incref(_modlist);
	}
}

Universe::ModuleList::~ModuleList()
{
	json_decref(_modlist);
}

bool
Universe::ModuleList::activate(void)
{
	json_t *value;
	const char *key, *s;
	bool failed;

	_activating = true;
	failed = false;
	json_object_foreach(_modlist, key, value)
	{
		if((s = json_string_value(value)))
		{
			fprintf(stderr, "ModuleList::%s: attempting to load module %s from '%s'\n", __FUNCTION__, key, s);
			if(!enable(s))
			{
				fprintf(stderr, "ModuleList::%s: failed to activate module %s\n", __FUNCTION__, key);
				failed = true;
			}
		}
		else
		{
			fprintf(stderr, "ModuleList::%s: entry '%s' is not a string value\n", __FUNCTION__, key);
		}
	}
	_activating = false;
	return !failed;
}

bool
Universe::ModuleList::enable(const char *name)
{
	char cname[Module::MAX_CANON_NAME + 1];
	Module *mod;

	if(!Module::nameIsValid(name))
	{
		fprintf(stderr, "ModuleList::%s: '%s' is not a valid module name\n", __FUNCTION__, name);
		return false;
	}
	Module::canonicalise(cname, sizeof(cname), name);
	if((mod = locateActiveModule(name)))
	{
		fprintf(stderr, "ModuleList::%s: '%s' matches an active module\n", __FUNCTION__, name);
		mod->release();
		return true;
	}
	if(!(mod = Module::load(_universe, name)))
	{
		fprintf(stderr, "ModuleList::%s: failed to load module %s\n", __FUNCTION__, name);
		return false;
	}
	fprintf(stderr, "ModuleList::%s: module %s [%s] has been loaded\n", __FUNCTION__, mod->name(), mod->canonicalName());
	/* the array will retain the module... */
	add(mod);
	/* ... so we release */
	mod->release();
	return true;
}

bool
Universe::ModuleList::disable(const char *name)
{
	Module *mod;

	if(!(mod = locateActiveModule(name)))
	{
		/* note that we still attempt to disable the module even if the name
		 * is not valid, just to be sure
		 */
		if(!Module::nameIsValid(name))
		{
			fprintf(stderr, "ModuleList::%s: '%s' is not a valid module name\n", __FUNCTION__, name);
			return false;
		}
		fprintf(stderr, "ModuleList::%s: '%s' does not match an active module\n", __FUNCTION__, name);
		return false;
	}
	/* we retain the module via locateActiveModule(), the module list
	 * should now release its pointer
	 */
	remove(mod);
	/* now release the module, allowing it to be unloaded */
	fprintf(stderr, "ModuleList::%s: releasing module '%s'\n", __FUNCTION__, mod->canonicalName());
	mod->release();
	return true;
}

Module *
Universe::ModuleList::locateActiveModule(const char *name)
{
	char cname[Module::MAX_CANON_NAME + 1];
	size_t index;
	Module *mod;

	if(!Module::nameIsValid(name))
	{
		return NULL;
	}
	Module::canonicalise(cname, sizeof(cname), name);
	for(index = 0; index < count(); index++)
	{
		if((mod = objectAtIndex(index)))
		{
			if(!strcmp(cname, mod->canonicalName()))
			{
				fprintf(stderr, "ModuleList::%s: matched module '%s' [%s] with '%s' [%s]\n", __FUNCTION__, name, cname, mod->name(), mod->canonicalName());
				/* still retained */
				return mod;
			}
			/* not a march, discard the reference */
			mod->release();
		}
	}
	return NULL;
}

size_t
Universe::ModuleList::add(Module *mod)
{
	size_t index;

	if((index = TArray<Module>::add(mod)) == NOTFOUND)
	{
		fprintf(stderr, "ModuleList::%s: module %s could not be added to array\n", __FUNCTION__, mod->canonicalName());
		return NOTFOUND;
	}
	fprintf(stderr, "ModuleList::%s: added %s at index %lu\n", __FUNCTION__, mod->canonicalName(), index);
	if(!_activating)
	{
		json_object_set_new(_modlist, mod->canonicalName(), json_string(mod->name()));
	}
	return index;
}

bool
Universe::ModuleList::remove(Module *mod)
{
	/* ensure we don't lose the module prematurely */
	mod->retain();
	if(!TArray<Module>::remove(mod))
	{
		mod->release();
		return false;
	}
	fprintf(stderr, "ModuleList::%s: removed %s\n", __FUNCTION__, mod->canonicalName());
	if(!_activating)
	{
		json_object_del(_modlist, mod->canonicalName());
	}
	/* now let it go */
	mod->release();
	return true;
}

void
Universe::ModuleList::objectWillBeDestroyed(void)
{
	_activating = true;
	Array::objectWillBeDestroyed();
}
