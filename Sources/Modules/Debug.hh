#ifndef DEBUG_HH_
# define DEBUG_HH_

# include "Nexus/Module.hh"
# include "Nexus/Commands.hh"

namespace Nexus
{
	namespace Modules
	{
		namespace Debug
		{
			class Debug: public ModuleImplementation
			{
				public:
					Debug(Universe *universe, Module *module);
				protected:
					virtual ~Debug();
				protected:
					static CommandEntry _commands[];
			};

			DECLARE_COMMAND(DUMP);
		}
	}
}

#endif /*!DEBUG_HH_*/
