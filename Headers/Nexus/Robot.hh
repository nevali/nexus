#ifndef NEXUS_ROBOT_HH_
# define NEXUS_ROBOT_HH_

# include "Player.hh"

namespace Nexus
{
	/* a Robot is a type of Player which is designated as being controlled
	 * by some external automaton of some kind rather than by a human typing
	 * commands
	 */
	class Robot: public Player
	{
		protected:
			friend class Thing;

			Robot(json_t *source): Player(source) {}
		public:
			virtual TypeID typeId(void) const { return ROBOT; }
			virtual bool isRobot(void) const { return true; }
	};


}

#endif /*NEXUS_ROBOT_HH_*/