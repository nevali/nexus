#ifndef NEXUS_LIMBO_HH_
# define NEXUS_LIMBO_HH_

# include "Zone.hh"

namespace Nexus
{
	/* Limbo is a special built-in zone which always has the ID #0 and is
	 * always available; it's used as a fallback location
	 *
	 * because Limbo exists only in memory, @teleporting an object to Limbo
	 * has the side-effect of causing it to exist outside of the normal root
	 * Zone, and so at next startup it won't be loaded automatically (it can
	 * still be manipulated and @teleported back into a normal Zone, but if
	 * you @examine it by ID after re-starting, it will be loaded, examined,
	 * and then discarded again)
	 * 
	 */
	class Limbo: public Zone
	{
		protected:
			friend class Universe;

			Limbo();
		public:
			virtual ID id(void) const;
			virtual Container *location(void) const __attribute__ (( warn_unused_result ));
	};
}

#endif /*!NEXUS_LIMBO_HH_*/