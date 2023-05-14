#ifndef COMMS_HH_
# define COMMS_HH_

# include "Nexus/Module.hh"
# include "Nexus/Commands.hh"

namespace Nexus
{
	namespace Modules
	{
		namespace Comms
		{
			class Comms: public ModuleImplementation
			{
				public:
					Comms(Universe *universe, Module *module);
				protected:
					virtual ~Comms();
				protected:
					static CommandEntry _commands[];
			};

			DECLARE_COMMAND(ECHO);
			DECLARE_COMMAND(EMIT);
		}
	}
}

#endif /*!COMMS_HH_*/
