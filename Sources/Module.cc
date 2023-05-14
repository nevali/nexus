#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include <dlfcn.h>

#include "Nexus/Universe.hh"
#include "Nexus/Module.hh"

using namespace Nexus;

bool
Module::nameIsValid(const char *name)
{
	if(!name ||!name[0])
	{
		return false;
	}
	if(strlen(name) > MAX_CANON_NAME)
	{
		return false;
	}
	for(; *name; name++)
	{
		if(!isalnum(*name) &&
			*name != '-')
		{
			return false;
		}
	}
	return true;
}

size_t
Module::canonicalise(char *buf, size_t buflen, const char *src)
{
	size_t n;

	n = 0;
	if(buflen > MAX_CANON_NAME + 1)
	{
		buflen = MAX_CANON_NAME + 1;
	}
	for(n = 0; n + 1 < buflen && src && *src; src++)
	{
		if(isalnum(*src) || *src == '-')
		{
			buf[n] = tolower(*src);
			n++;
		}
	}
	buf[n] = 0;
	return n;
}


Module *
Module::load(Universe *universe, const char *name)
{
	fprintf(stderr, "Module::%s: attempting to load '%s'\n", __FUNCTION__, name);

	Module *mod = new Module(universe, name);

	if(!mod->_impl)
	{
		fprintf(stderr, "Module::%s: '%s' is not a valid module\n", __FUNCTION__, name);
		mod->release();
		return NULL;
	}
	return mod;
}

Module::Module(Universe *universe, const char *name):
	WARP::Flux::Object(),
	_universe(universe),
	_handle(NULL),
	_name(NULL),
	_cname(""),
	_path(NULL),
	_impl(NULL)
{
	size_t len;
	ModuleImplementation *(*callback)(Universe *universe, Module *mod);

	_name = strdup(name);
	canonicalise(_cname, sizeof(_cname), name);
	len = strlen(name) + 64;
	_path = (char *) calloc(1, len + 1),
	/* XXX FIXME */
	snprintf(_path, len, "Sources/Modules/.libs/%s.so", name);
	_handle = dlopen(_path, RTLD_LOCAL|RTLD_NOW);
	if(!_handle)
	{
		fprintf(stderr, "Module::%s: %s: %s\n", __FUNCTION__, _path, dlerror());
		return;
	}
	callback = (ModuleImplementation *(*)(Universe *, Module *)) dlsym(_handle, "nexusModuleImplementation");
	if(!callback)
	{
		fprintf(stderr, "Module::%s: %s: module callback not found\n", __FUNCTION__, _path);
		return;
	}
	_impl = callback(_universe, this);
	if(!_impl)
	{
		fprintf(stderr, "Module::%s: %s: module callback failed\n", __FUNCTION__, _path);
		return;
	}
}

Module::~Module()
{
	if(_impl)
	{
		fprintf(stderr, "Module::%s: releasing implementation of %s\n", __FUNCTION__, _cname);
		_impl->release();
	}
	if(_handle)
	{
		fprintf(stderr, "Module::%s: unloading '%s'\n", __FUNCTION__, _path);
		dlclose(_handle);
	}
	free(_name);
	free(_path);
	_universe = NULL;
}

const char *
Module::name(void) const
{
	return _name;
}

const char *
Module::canonicalName(void) const
{
	return _cname;
}

const char *
Module::path(void) const
{
	return _path;
}
