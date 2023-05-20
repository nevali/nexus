#ifndef NEXUS_PLAYER_HH_
# define NEXUS_PLAYER_HH_

# include "WARP/Flux/Buffer.hh"
# include "WARP/Flux/Channel.hh"
# include "WARP/Flux/BufferDelegate.hh"

# include "Actor.hh"

namespace Nexus
{
	/* a Player is you: a person interactively exploring (or building) the
	 * universe, or saved data about someone who has previously
	 *
	 * in principle there can be any number of Players in a Universe, as
	 * long as their canonical names do not clash
	 */
	class Player: public Actor, public WARP::Flux::BufferDelegate
	{
		protected:
			friend class Thing;
			friend class Universe;

			Player(json_t *source): Actor(source), _connected(false), _inputBuffer(NULL), _channel(NULL) {}
			virtual ~Player();
			virtual void sendPrompt(void);
			virtual void connectChannels(void);
		public:
			virtual TypeID typeId(void) const { return PLAYER; }
			virtual bool isPlayer(void) const { return true; }
			
			virtual void send(const char *string);
			virtual void flush(void);
			virtual void connect(void);
			virtual void connect(WARP::Flux::Buffer *inputBuffer);
			virtual void connect(WARP::Flux::Channel *channel);
			virtual void disconnect(void);
			virtual bool connected(void) const { return _connected; }
		public:
			/* BufferDelegate */
			virtual void bufferFilled(WARP::Flux::Object *sender, WARP::Flux::Buffer *buffer, char *base, size_t *nbytes);
			virtual void sourceClosed(WARP::Flux::Object *sender,WARP::Flux::Object *source);
		protected:
			bool _connected;
			WARP::Flux::Buffer *_inputBuffer;
			WARP::Flux::Channel *_channel;
	};

}

#endif /*NEXUS_PLAYER_HH_*/