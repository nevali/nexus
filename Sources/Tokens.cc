#include <cstdio>

#include <cstring>
#include <cctype>
#include <cstdlib>

#include "WARP/Flux/Diagnostics.hh"

#include "Nexus/Tokens.hh"

using namespace Nexus;
using namespace WARP::Flux::Diagnostics;

static inline bool
iswhitespace(int ch)
{
	if(isblank(ch)) return true;
	if(ch == '\n' || ch == '\r') return true;
	return false;
}

Tokens::Tokens(const char *source, size_t length):
	WARP::Flux::PointerArray(),
	_source(NULL),
	_length(length),
	_quoted(0)
{
	size_t start, pos;
	bool e;

	_source = strdup(source);
	start = 0;
	if(_length == (size_t) -1)
	{
		_length = strlen(_source);
	}
	e = false;
	while(start < _length)
	{
		if(!Token::findStart(_source, &start, _length))
		{
			break;
		}
		pos = start;
		Token::findEnd(_source, &pos, _length, &e, &_quoted);
		if(pos > start)
		{
			addPointer(new Token(_source, start, pos));
		}
		start = pos;
	}
}

Tokens::~Tokens()
{
	free(_source);
}

const Token *
Tokens::tokenAtIndex(size_t index) const
{
	return static_cast<const Token *>(pointerAtIndex(index));
}

const char *
Tokens::raw(size_t index) const
{
	const Token *token = tokenAtIndex(index);

	if(token)
	{
		return token->raw();
	}
	return NULL;
}

const char *
Tokens::processed(size_t index) const
{
	const Token *token = tokenAtIndex(index);

	if(token)
	{
		return token->processed();
	}
	return NULL;
}

void
Tokens::pointerWasRemoved(void *ptr) const
{
	Token *token = static_cast<Token *>(ptr);

	if(token)
	{
		delete token;
	}
}

void
Tokens::dump(void)
{
	size_t n;

	for(n = 0; n < count(); n++)
	{
		::debugf("Token %lu = '%s'\n", n, tokenAtIndex(n)->_raw);
	}
}

Token::Token(const char *source, size_t start, size_t end):
	_start(&source[start]),
	_length(end - start),
	_raw(NULL),
	_processed(NULL)
{
	_raw = (char *) calloc(1, _length + 1);
	memcpy(_raw, &(source[start]), _length);
	process();
//	::debugf("Token::%s: start=%lu, end=%lu, length=%lu, raw='%s', processed='%s'\n", __FUNCTION__, start, end, _length, _raw, _processed);
}

Token::~Token()
{
	free(_raw);
	free(_processed);
}

/* find the start of the next token (skip any whitespace) */
bool
Token::findStart(const char *source, size_t *pos, size_t length)
{
	for(; *pos < length && iswhitespace(source[*pos]); (*pos)++)
	{
	}
	if(*pos >= length)
	{
		return false;
	}
	return true;
}

/* find the end of the current token (find next whitespace or NUL) */
void
Token::findEnd(const char *source, size_t *pos, size_t length, bool *escaped, int *quoted)
{
	int ch;

	while(*pos < length)
	{
		ch = source[*pos];
		if(*escaped)
		{
			/* we were escaped in the previous pass, just move past this (escaped)
				* character, whatever it is, and resetting the flag, and then continue
				* as normal
				*/
			*escaped = false;
			(*pos)++;
			continue;
		}
		if(ch == '\\')
		{
			/* an escape character: set the flag and continue */
			*escaped = true;
			(*pos)++;
			continue;
		}
		if(*quoted)
		{
			/* currently quoted */
			if(ch == *quoted)
			{
				/* end of quoted section */
				*quoted = 0;
			}
			(*pos)++;
			/* just consume the characters until we reach the end */
			continue;
		}
		/* not quoted */
		/* encountered a quoting character - remember it and continue */
		if((ch == '\'' || ch == '"'))
		{
			*quoted = ch;
			(*pos)++;
			continue;
		}
		/* not quoted - whitespace means the end of the argument */
		if(iswhitespace(ch))
		{
			return;
		}
		/* not an escape, quote, or whitespace character, just continue */
		(*pos)++;
	}
}

const char *
Token::raw(void) const
{
	return _raw;
}


const char *
Token::processed(void) const
{
	return _processed;
}

/* generate the 'processed' version of the token from the 'raw' buffer, which
 * means applying escapes and filtering out quotes
 *
 * that's it
 */
void
Token::process(void)
{
	size_t c;
	char *p;
	bool esc;

	/* without substitutions, the processed version will be at most the same
	 * length as the raw string
	 */
	_processed = (char *) calloc(1, _length + 1);
	c = 0;
	esc = false;
	for(p = _processed; _raw[c]; c++)
	{
		if(esc)
		{
			esc = false;
			/* copy the escaped character */
			*p = _raw[c];
			p++;
			continue;
		}
		if(_raw[c] == '\\')
		{
			esc = true;
			/* skip the actual escape character */
			continue;
		}
		if(_raw[c] == '"' || _raw[c] == '\'')
		{
			/* skip these unless escaped */
			continue;
		}
		*p = _raw[c];
		p++;
	}
	*p = 0;
}
