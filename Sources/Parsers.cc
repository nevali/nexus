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
	free(_commands);
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
Parser::remove(const CommandEntry &entry)
{
	for(size_t c = 0; c < _ncommands; c++)
	{
		if(!strcasecmp(_commands[c].name, entry.name) &&
			_commands[c].constructor == entry.constructor)
		{
//			fprintf(stderr, "Parser::%s: removing entry %lu ('%s') from commands\n", __FUNCTION__, c, entry.name);
			if(c + 1 < _ncommands)
			{
				/* move all of the following commands up a place */
				memmove(&(_commands[c]), &(_commands[c + 1]), sizeof(CommandEntry) * (_ncommands - c - 1));
			}
			else
			{
				/* just terminate here */
				memset(&(_commands[c]), 0, sizeof(CommandEntry));
			}
			_ncommands--;
#if 0			
			fprintf(stderr, "----- List is now -----\n");
			for(size_t c = 0; c < _ncommands; c++)
			{
				fprintf(stderr, "  %lu => %s\n", c, _commands[c].name);
			}
			fprintf(stderr, "----------------------\n");
#endif
			return true;
		}
	}
	return false;
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
		else
		{
			fprintf(stderr, "Parser::%s: registered command '%s'\n", __FUNCTION__, entry[c].name);
		}
	}
	return r;
}

bool
Parser::remove(const CommandEntry *entry)
{
	bool r;

	r = true;
	for(size_t c = 0; entry[c].name; c++)
	{
		if(!remove(entry[c]))
		{
			fprintf(stderr, "Parser::%s: failed to remove command entry '%s'\n", __FUNCTION__, entry[c].name);
			r = false;
		}
		else
		{
			fprintf(stderr, "Parser::%s: unregistered command '%s'\n", __FUNCTION__, entry[c].name);
		}
	}
	return r;
}
