#ifndef NEXUS_ACTOR_HH_
# define NEXUS_ACTOR_HH_

# include "Container.hh"

namespace Nexus
{
	class Actor: public Container
	{
		protected:
			friend class Thing;

			Actor(json_t *source): Container(source) {}
		public:
			virtual TypeID typeId(void) const { return ACTOR; }
			virtual bool isActor(void) const { return true; }
			virtual bool perform(const char *command);
	};
}

#endif /*!NEXUS_ACTOR_HH_*/
