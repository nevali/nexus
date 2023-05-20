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
			virtual bool activate(void);
			virtual bool deactivate(void);
			virtual void activated(void);
			virtual void deactivated(void);
			virtual void connectChannels(void);
			virtual void disconnectChannels(void);
			virtual bool connectToChannel(Channel *channel);
			virtual bool disconnectFromChannel(Channel *channel);
		public:
			virtual TypeID typeId(void) const { return ACTOR; }
			virtual bool isActor(void) const { return true; }
			virtual bool perform(const char *command);
			virtual bool setFlag(const char *flag, bool set = true);
		protected:
			virtual void applyFlag(const char *flag, bool set);
	};
}

#endif /*!NEXUS_ACTOR_HH_*/
