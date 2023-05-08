#ifndef NEXUS_COMMANDS_HH_
# define NEXUS_COMMANDS_HH_

# include "WARP/Flux/Object.hh"

namespace Nexus
{
	class Universe;
	class Actor;
	class Command;
	class Parser;

	struct CommandEntry
	{
		typedef enum
		{
			NONE = 0,
			HIDDEN = (1<<0),
			UNAMBIGUOUS = (1<<1)
		} Flags;

		typedef Command *(*Constructor)(Parser *parser, const char *commandLine);

		/* the name of the command */
		const char *name;
		/* the command flags */
		unsigned flags;
		/* the Constructor callback */
		Constructor constructor;
		/* the short help text */
		const char *desc;
	};

	/* a Parser encapsulates a command parser (or command mode) */
	class Parser: public WARP::Flux::Object
	{
		protected:
			friend class Command;
			
			Parser(Universe *universe);
			virtual ~Parser();
		public:
			virtual Command *parse(Actor *who, const char *commandLine) = 0;
			virtual size_t commandCount(void) const { return _ncommands; }
			virtual CommandEntry *commandAtIndex(size_t index)
			{
				if(index < 0 || index >= _ncommands)
				{
					return NULL;
				}
				return &(_commands[index]);
			}

		protected:
			bool add(const CommandEntry *entry);
			bool add(const CommandEntry &entry);
		protected:
			Universe *_universe;
			CommandEntry *_commands;
			size_t _ncommands;
	};

	/* BuiltinsParser handles the built-in @-commands */
	class BuiltinsParser: public Parser
	{
		public:
			BuiltinsParser(Universe *universe);
		public:
			virtual Command *parse(Actor *who, const char *commandLine);
	};

	/* subclasses provide different implementations, instances
	 * encapsulate a parsed command-line ready to be executed
	 * on behalf of some Actor
	 * 
	 */
	class Command: public WARP::Flux::Object
	{
		protected:
			Command(Parser *parser, const char *commandLine);
			virtual ~Command();
		public:
			virtual bool parse(Actor *actor);
			virtual bool execute(Actor *actor) = 0;
		protected:
			Parser *_parser;
			Universe *_universe;
			char *_argsBuf;
			int _argc;
			char *_argv[16];
	};

# define DECLARE_COMMAND_(name, extra) \
	class name: public Command \
	{ \
		public: \
			static Command *construct(Parser *parser, const char *commandLine) { return new name(parser, commandLine); } \
			name(Parser *parser, const char *commandLine): Command(parser, commandLine) { } \
			virtual bool execute(Actor *actor); \
			extra \
	}
}

# define DECLARE_COMMAND(name) DECLARE_COMMAND_(name, /* */)

#endif /*!NEXUS_COMMANDS_HH_*/
