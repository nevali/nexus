#ifndef NEXUS_VARIABLE_HH_
# define NEXUS_VARIABLE_HH_

# include "Thing.hh"

namespace Nexus
{
	/* a Variable represents a settable value of some kind
	 *
	 * a Variable can EITHER be set to a JSON value (of any kind)
	 *  OR
	 * it can be set to a block of text
	 * 
	 * if a JSON value is set then value() returns it, and
	 * textValue() returns its text value (or NULL if it's
	 * not text)
	 * 
	 * if a text value is set then value() returns NULL, and
	 * textValue() returns it
	 */
	class Variable: public Thing
	{
		protected:
			friend class Thing;

			Variable(json_t *source): Thing(source) {}
		public:
			virtual TypeID typeId(void) const { return VARIABLE; }
			virtual bool isVariable(void) const { return true; }
		public:
			virtual const char *textValue(void) const;
			virtual json_t *value(void);
			virtual bool setText(const char *text);
			virtual bool setValue(json_t *json);
		protected:
			virtual void examineSections(Actor *who);
	};
}

#endif /*!NEXUS_VARIABLE_HH_*/
