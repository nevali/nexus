#ifndef NEXUS_COMMANDS_HH_
# define NEXUS_COMMANDS_HH_

# include <cstddef>

# include "WARP/Flux/Object.hh"

namespace Nexus
{
	class Universe;
	class Actor;
	class Command;
	class Parser;
	class Tokens;

	struct ExecutionContext
	{
		/* Who is performing the command */
		Actor *who;
		/* The boolean result of the command (true = succeeded, false = failed) */
		bool result;
		/* The tokens that made up this command */
		Tokens *tokens;
		
		ExecutionContext(Actor *actor);
		~ExecutionContext();
		void setTokens(Tokens *newTokens);
	};

	struct CommandEntry
	{
		typedef enum
		{
			NONE = 0,
			HIDDEN = (1<<0),
			UNAMBIGUOUS = (1<<1)
		} Flags;

		typedef Command *(*Constructor)(Parser *parser, Tokens *tokens);

		/* the name of the command */
		const char *name;
		/* the command flags */
		unsigned flags;
		/* the flags the calling Actor must have */
		unsigned actorFlags;
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
			virtual Command *parse(ExecutionContext *context, const char *commandLine, size_t length) = 0;
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
			friend class ModuleImplementation;

			bool add(const CommandEntry *entry);
			bool remove(const CommandEntry *entry);
			bool add(const CommandEntry &entry);
			bool remove(const CommandEntry &entry);
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
			virtual Command *parse(ExecutionContext *context, const char *commandLine, size_t length);
	};

	/* subclasses provide different implementations, instances
	 * encapsulate a parsed command-line ready to be executed
	 * on behalf of some Actor
	 * 
	 */
	class Command: public WARP::Flux::Object
	{
		protected:
			Command(Parser *parser, Tokens *tokens);
			virtual ~Command();
		public:
			virtual bool execute(ExecutionContext *context) = 0;
		protected:
			int argc(void) const;
			const char *argv(size_t index) const;
		protected:
			Parser *_parser;
			Universe *_universe;
			Tokens *_tokens;
	};

# define DECLARE_COMMAND_(name, extra) \
	class name: public Command \
	{ \
		public: \
			static Command *construct(Parser *parser, Tokens *tokens) { return new name(parser, tokens); } \
			name(Parser *parser, Tokens *tokens): Command(parser, tokens) { } \
			virtual bool execute(ExecutionContext *context); \
			extra \
	}
# define DECLARE_COMMAND(name) DECLARE_COMMAND_(name, /* */)

	namespace Builtins
	{
		DECLARE_COMMAND(QUIT);
		DECLARE_COMMAND(COMMANDS);
		DECLARE_COMMAND(MODULES);
		DECLARE_COMMAND(ENABLE);
		DECLARE_COMMAND(DISABLE);
	};
}

#endif /*!NEXUS_COMMANDS_HH_*/
