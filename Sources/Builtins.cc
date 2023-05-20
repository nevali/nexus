#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "WARP/Flux/Diagnostics.hh"

#include "Nexus/Universe.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Actor.hh"
#include "Nexus/Player.hh"
#include "Nexus/Zone.hh"
#include "Nexus/Room.hh"
#include "Nexus/Variable.hh"
#include "Nexus/Portal.hh"
#include "Nexus/Tokens.hh"

using namespace Nexus;
using namespace WARP::Flux::Diagnostics;

BuiltinsParser::BuiltinsParser(Universe *universe):
	Parser(universe)
{
	CommandEntry c[] = {
		{ "BYE", CommandEntry::UNAMBIGUOUS|CommandEntry::HIDDEN, 0, Builtins::QUIT::construct, NULL },

		{ "COMMANDS", CommandEntry::NONE, 0, Builtins::COMMANDS::construct, "List available commands" },
		
		{ "DISABLE", CommandEntry::NONE, Thing::ENGINEER, Builtins::DISABLE::construct,"Disable a module" },
		
		{ "ENABLE", CommandEntry::NONE, Thing::ENGINEER, Builtins::ENABLE::construct, "Enable a module" },
		
		{ "EXIT", CommandEntry::UNAMBIGUOUS|CommandEntry::HIDDEN, 0, Builtins::QUIT::construct, NULL },

		{ "LOGOUT", CommandEntry::UNAMBIGUOUS|CommandEntry::HIDDEN, 0, Builtins::QUIT::construct, NULL },
		{ "LSMOD", CommandEntry::UNAMBIGUOUS|CommandEntry::HIDDEN, Thing::ENGINEER, Builtins::MODULES::construct, NULL },

		{ "MODULES", CommandEntry::NONE, Thing::ENGINEER, Builtins::MODULES::construct, "List active modules" },
		
		{ "QUIT", CommandEntry::UNAMBIGUOUS, 0, Builtins::QUIT::construct, "End your session" },
		{ NULL, CommandEntry::NONE, 0, NULL, NULL }
	};
	add(c);
}

Command *
BuiltinsParser::parse(Actor *who, const char *commandLine)
{
	Tokens *tokens;
	const char *cmdstr;
	size_t cmdlen;
	CommandEntry *partial, *exact;
	Command *command;
	bool clash;
	size_t c;

	command = NULL;
	partial = NULL;
	exact = NULL;
	clash = false;
	/* All built-in start with an '@' character followed by one or more
	 * alphabetic characters; for our purposes we consider anything from
	 * the '@' to the first whitespace OR forward-slash to the command
	 * name
	 */
	tokens = new Tokens(commandLine);
	cmdstr = tokens->raw(0);
	if(!cmdstr || cmdstr[0] != '@')
	{
		tokens->release();
		return NULL;
	}
	/* skip the '@' */
	cmdstr++;
	cmdlen = strlen(cmdstr);
	/* commands are partial-matched unless flagged as being full-match-only
	 * an exact match always takes precedence
	 */
	for(c = 0; c < _ncommands; c++)
	{
		if(!strcasecmp(_commands[c].name, cmdstr))
		{
			exact = &(_commands[c]);
			break;
		}
		if(!(_commands[c].flags & CommandEntry::UNAMBIGUOUS))
		{
			if(!strncasecmp(_commands[c].name, cmdstr, cmdlen))
			{
				if(partial)
				{
					clash = true;
				}
				else
				{
					partial = &(_commands[c]);
				}
			}
		}
	}
	if(!exact)
	{
		/* if there was more than one partial match, we list the available
		* matches
		*/
		if(clash)
		{
			bool first = true;
			who->sendf("Multiple command matches for '@%s':\n  ", cmdstr);
			for(c = 0; c < _ncommands; c++)
			{
				if(!(_commands[c].flags & CommandEntry::UNAMBIGUOUS))
				{
					if(!strncasecmp(_commands[c].name, cmdstr, cmdlen))
					{
						who->sendf("%s@%s", (first ? "" : ", "), _commands[c].name);
						first = false;
					}
				}
			}
			who->send("\n");
			tokens->release();
			return NULL;
		}
		/* if there was exactly one partial match, use it */
		if(partial)
		{
			exact = partial;
		}
		else
		{
			who->sendf("Sorry, the command '@%s' was not recognised.\n", cmdstr);
			tokens->release();
			return NULL;
		}
	}
	/* once we've located the matching command entry, invoke its
	 * constructor callback to obtain a Command instance
	 */
	if(!exact->constructor)
	{
		who->sendf("Sorry, the '@%s' command is not implemented.\n", exact->name);
		tokens->release();
		return NULL;
	}
	command = exact->constructor(this, tokens);
	tokens->release();
	if(!command)
	{
		::debugf("BuiltinsParser::%s: constructor for command '%s' failed\n", __FUNCTION__, exact->name);
		return NULL;
	}
	/* parsing succeeded, return the Command so that it can be executed */
	return command;
}
