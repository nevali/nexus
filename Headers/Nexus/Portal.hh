#ifndef NEXUS_PORTAL_HH_
# define NEXUS_PORTAL_HH_

# include "Thing.hh"

namespace Nexus
{
	class Container;

	/* a Portal connects one location (usually a Room) to another - i.e.,
	 * it is a (one-way) door or exit
	 */
	class Portal: public Thing
	{
		protected:
			friend class Thing;

			Portal(json_t *source): Thing(source) {}
		public:
			virtual TypeID typeId(void) const { return PORTAL; }
			virtual bool isPortal(void) const { return true; }
	};

}

#endif /*NEXUS_PORTAL_HH_*/
