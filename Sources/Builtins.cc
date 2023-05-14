#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "Nexus/Universe.hh"
#include "Nexus/Commands.hh"
#include "Nexus/Actor.hh"
#include "Nexus/Player.hh"
#include "Nexus/Zone.hh"
#include "Nexus/Room.hh"
#include "Nexus/Variable.hh"
#include "Nexus/Portal.hh"

using namespace Nexus;

BuiltinsParser::BuiltinsParser(Universe *universe):
	Parser(universe)
{
	CommandEntry c[] = {
		{ "BYE", CommandEntry::UNAMBIGUOUS|CommandEntry::HIDDEN, Builtins::QUIT::construct, NULL },

		{ "COMMANDS", CommandEntry::NONE, Builtins::COMMANDS::construct, "List available commands" },
		
		{ "DISABLE", CommandEntry::NONE, Builtins::DISABLE::construct, "Disable a module" },
		
		{ "ENABLE", CommandEntry::NONE, Builtins::ENABLE::construct, "Enable a module" },
		
		{ "EXIT", CommandEntry::UNAMBIGUOUS|CommandEntry::HIDDEN, Builtins::QUIT::construct, NULL },

		{ "LOGOUT", CommandEntry::UNAMBIGUOUS|CommandEntry::HIDDEN, Builtins::QUIT::construct, NULL },
		{ "LSMOD", CommandEntry::UNAMBIGUOUS|CommandEntry::HIDDEN, Builtins::MODULES::construct, NULL },

		{ "MODULES", CommandEntry::NONE, Builtins::MODULES::construct, "List active modules" },
		
		{ "QUIT", CommandEntry::UNAMBIGUOUS, Builtins::QUIT::construct, "End your session" },
		{ NULL, CommandEntry::NONE, NULL, NULL }
	};
	add(c);
}

Command *
BuiltinsParser::parse(Actor *who, const char *commandLine)
{
	char cmdbuf[32];
	const char *start;
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
	if(!commandLine || *commandLine != '@')
	{
		return NULL;
	}
	start = commandLine;
	commandLine++;
	for(c = 0; *commandLine && c < sizeof(cmdbuf) - 1; commandLine++)
	{
		if(!*commandLine || *commandLine == '/' || isblank(*commandLine) || *commandLine == '\r' || *commandLine == '\n')
		{
			break;
		}
		cmdbuf[c] = *commandLine;
		c++;
	}
	cmdbuf[c] = 0;

	/* commands are partial-matched unless flagged as being full-match-only
	 * an exact match always takes precedence
	 */
	for(c = 0; c < _ncommands; c++)
	{
		if(!strcasecmp(_commands[c].name, cmdbuf))
		{
			exact = &(_commands[c]);
			break;
		}
		if(!(_commands[c].flags & CommandEntry::UNAMBIGUOUS))
		{
			if(!strncasecmp(_commands[c].name, cmdbuf, strlen(cmdbuf)))
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
			who->sendf("Multiple command matches for '@%s':\n", cmdbuf);
			return NULL;
		}
		/* if there was exactly one partial match, use it */
		if(partial)
		{
			exact = partial;
		}
		else
		{
			who->sendf("Sorry, the command '@%s' was not recognised.\n", cmdbuf);
			return NULL;
		}
	}
	/* once we've located the matching command entry, invoke its
	 * constructor callback to obtain a Command instance
	 */
	if(!exact->constructor)
	{
		who->sendf("Sorry, the '@%s' command is not implemented.\n", exact->name);
		return NULL;
	}
	command = exact->constructor(this, start);
	if(!command)
	{
		fprintf(stderr, "BuiltinsParser::%s: constructor for command '%s' failed\n", __FUNCTION__, exact->name);
		return NULL;
	}
	/* now we have the Command instance, request it parse its arguments */
	if(!command->parse(who))
	{
		command->release();
		return NULL;
	}
	/* parsing succeeded, return the Command so that it can be executed */
	return command;
}
