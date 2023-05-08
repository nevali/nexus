#include <cstring>
#include <cstdlib>
#include <cctype>

#include "Nexus/Commands.hh"
#include "Nexus/Universe.hh"

using namespace Nexus;

Command::Command(Parser *parser, const char *commandLine):
	Object(),
	_parser(parser),
	_argc(0)
{
	_argsBuf = strdup(commandLine);
	_parser->retain();
	_universe = _parser->_universe;
	_universe->retain();
}

Command::~Command()
{
	_universe->release();
	_parser->release();
	free(_argsBuf);
}

bool
Command::parse(Actor *who)
{
	char *t, *cmdbuf;

	(void) who;

	memset(_argv, 0, sizeof(_argv));
	_argc = 0;
	cmdbuf = _argsBuf;
	for(t = cmdbuf; (size_t) _argc < (sizeof(_argv) / sizeof(char *)) - 1 && t && *t; cmdbuf = t)
	{
		while(*cmdbuf && isspace(*cmdbuf))
		{
			cmdbuf++;
		}
		if(!*cmdbuf)
		{
			break;
		}
		/* this needs to be much more nuanced */
		for(t = cmdbuf; *t && !isspace(*t); t++)
		{
			if(!_argc && t > cmdbuf && (*t == '/' || *t == '-'))
			{
				_argv[_argc] = cmdbuf;
				_argc++;
				cmdbuf = t;
			}
		}
		if(*t)
		{
			*t = 0;
			t++;
		}
		_argv[_argc] = cmdbuf;
		_argc++;		
	}
	_argv[_argc] = NULL;
	return true;
}