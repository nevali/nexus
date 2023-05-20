#ifndef NEXUS_TOKENS_HH_
# define NEXUS_TOKENS_HH_

# include "WARP/Flux/Array.hh"

namespace Nexus
{
	class Tokens;

	/* This is POD with a constructor and accessors */
	struct Token
	{
		public:
			size_t length(void) const;
			const char *remainder(void) const;
			const char *raw(void) const;
			const char *processed(void) const;
		protected:
			friend class Tokens;

			static bool findStart(const char *source, size_t *pos, size_t length);
			static void findEnd(const char *source, size_t *pos, size_t length, bool *escaped, int *quoted);

			Token(const char *source, size_t start, size_t end);
			~Token();
		private:
			const char *_start;
			size_t _length;
			char *_raw;
			char *_processed;
		private:
			void process(void);
	};

	class Tokens: public WARP::Flux::PointerArray
	{
		public:
			Tokens(const char *source, size_t length = -1);
			const Token *tokenAtIndex(size_t index) const;
			virtual void dump(void);
			/* accessors for token text */
			virtual const char *raw(size_t index) const;
			virtual const char *processed(size_t index) const;
		protected:
			virtual ~Tokens();
			virtual void pointerWasRemoved(void *ptr) const;
		private:
			char *_source;
			size_t _length;
			int _quoted;
	};
}

#endif /*!NEXUS_TOKENS_HH_*/