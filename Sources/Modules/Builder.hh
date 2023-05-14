#ifndef BUILDER_HH_
# define BUILDER_HH_

# include "Nexus/Module.hh"
# include "Nexus/Commands.hh"

namespace Nexus
{
	namespace Modules
	{
		namespace Builder
		{
			class Builder: public ModuleImplementation
			{
				public:
					Builder(Universe *universe, Module *module);
				protected:
					virtual ~Builder();
				protected:
					static CommandEntry _commands[];
			};

			DECLARE_COMMAND_(CREATE,
				bool createThing(Actor *who, const char *name);
				bool createZone(Actor *who, const char *name);
				bool createRoom(Actor *who, const char *name);
				bool createPortal(Actor *who, const char *name);
			);
			DECLARE_COMMAND(DESTROY);
			DECLARE_COMMAND(EXAMINE);
			DECLARE_COMMAND(FLAG);
			DECLARE_COMMAND(GO);
			DECLARE_COMMAND(LIST);
			DECLARE_COMMAND(LOOK);
			DECLARE_COMMAND(RENAME);
			DECLARE_COMMAND(SET);
			DECLARE_COMMAND(TELEPORT);
		}
	}
}

#endif /*!BUILDER_HH_*/