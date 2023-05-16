#include <cstring>
#include <cstdlib>
#include <cctype>

#include "Nexus/Commands.hh"
#include "Nexus/Universe.hh"
#include "Nexus/Tokens.hh"

using namespace Nexus;

Command::Command(Parser *parser, Tokens *tokens):
	Object(),
	_parser(parser),
	_tokens(tokens)
{
	_tokens->retain();
	_parser->retain();
	_universe = _parser->_universe;
	_universe->retain();
}

Command::~Command()
{
	if(_tokens)
	{
		_tokens->release();
	}
	if(_universe)
	{
		_universe->release();
	}
	if(_parser)
	{
		_parser->release();
	}
}

int
Command::argc(void) const
{
	return _tokens->count();
}

const char *
Command::argv(size_t n) const
{
	return _tokens->processed(n);
}
