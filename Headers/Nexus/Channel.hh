#ifndef NEXUS_CHANNEL_HH_
# define NEXUS_CHANNEL_HH_

# include "WARP/Flux/Array.hh"
# include "WARP/Flux/Diagnostics.hh"

# include "Thing.hh"

namespace Nexus
{
	class Actor;

	class Channel: public Thing
	{
		protected:
			friend class Thing;

			Channel(json_t *source);
			virtual ~Channel();
		public:
			virtual TypeID typeId(void) const { return CHANNEL; }
			virtual bool isChannel(void) const { return true; }
			virtual void send(const char *string);
			virtual void emit(const char *string);
		protected:
			friend class Actor;

			virtual bool connect(Actor *actor);
			virtual bool disconnect(Actor *actor);
		private:
			WARP::Flux::TArray<Actor> *_currentMembers;
	};
}

#endif /*!NEXUS_CHANNEL_HH_*/
