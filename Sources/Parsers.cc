#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "Nexus/Universe.hh"
#include "Nexus/Commands.hh"

using namespace Nexus;

Parser::Parser(Universe *universe):
	Object(),
	_universe(universe),
	_commands(NULL),
	_ncommands(0)
{
}

Parser::~Parser()
{
}

bool
Parser::add(const CommandEntry &entry)
{
	CommandEntry *p;

	for(size_t c = 0; c < _ncommands; c++)
	{
		if(!strcasecmp(_commands[c].name, entry.name))
		{
			_commands[c] = entry;
			return true;
		}
	}
	p = (CommandEntry *) realloc(_commands, (_ncommands + 2) * sizeof(CommandEntry));
	_commands = p;
	p[_ncommands] = entry;
	_ncommands++;
	memset(&(p[_ncommands]), 0, sizeof(CommandEntry));
	return true;
}

bool
Parser::add(const CommandEntry *entry)
{
	bool r;

	r = true;
	for(size_t c = 0; entry[c].name; c++)
	{
		if(!add(entry[c]))
		{
			fprintf(stderr, "Parser::%s: failed to add command entry '%s'\n", __FUNCTION__, entry[c].name);
			r = false;
		}
	}
	return r;
}
