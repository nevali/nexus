#ifndef NEXUS_HOLOGRAM_HH_
# define NEXUS_HOLOGRAM_HH_

# include "Actor.hh"

namespace Nexus
{
	/* a Hologram is an Actor which is controlled by something else - this
	 * could be a typical NPC, or it could be something like a "familiar"
	 * which is bound to a particular player
	 */
	class Hologram: public Actor
	{
		protected:
			friend class Thing;

			Hologram(json_t *source): Actor(source) {}
		public:
			virtual TypeID typeId(void) const { return HOLOGRAM; }
			virtual bool isHologram(void) const { return true; }
	};
}

#endif /*!NEXUS_HOLOGRAM_HH_*/
