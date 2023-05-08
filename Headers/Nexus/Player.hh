#ifndef NEXUS_PLAYER_HH_
# define NEXUS_PLAYER_HH_

# include "Actor.hh"

namespace Nexus
{
	/* a Player is you: a person interactively exploring (or building) the
	 * universe, or saved data about someone who has previously
	 *
	 * in principle there can be any number of Players in a Universe, as
	 * long as their canonical names do not clash
	 */
	class Player: public Actor
	{
		protected:
			friend class Thing;
			friend class Universe;

			Player(json_t *source): Actor(source), _connected(false) {}
		public:
			virtual TypeID typeId(void) const { return PLAYER; }
			virtual bool isPlayer(void) const { return true; }
			
			virtual void send(const char *string);
			virtual void connect(void);
			virtual void disconnect(void);
			virtual bool connected(void) const { return _connected; }
		protected:
			bool _connected;
	};

}

#endif /*NEXUS_PLAYER_HH_*/