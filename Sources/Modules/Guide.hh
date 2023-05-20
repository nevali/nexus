#ifndef GUIDE_HH_
# define GUIDE_HH_

# include "Nexus/Module.hh"
# include "Nexus/Commands.hh"

namespace Nexus
{
	namespace Modules
	{
		namespace Guide
		{
			class Guide: public ModuleImplementation
			{
				public:
					Guide(Universe *universe, Module *module);
				protected:
					virtual ~Guide();
				protected:
					static CommandEntry _commands[];
			};

			DECLARE_COMMAND_(HELP,
				bool intro(ExecutionContext *ctx);
				bool disclaimers(ExecutionContext *ctx);
			);
		}
	}
}

#endif /*!GUIDE_HH_*/
