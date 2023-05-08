#include "Nexus/Limbo.hh"

using namespace Nexus;

Limbo::Limbo():
	Zone(NULL)
{
	json_t *flags;

	_id = ID_LIMBO;
	_typeId = ZONE;

	json_object_set_new(_obj, "type", json_string("zone"));
	json_object_set_new(_obj, "name", json_string("Limbo"));
	json_object_set_new(_obj, "description", json_string(
		"You find yourself in Limbo, a place which technically doesn't exist.\n"
		"\n"
		"If you are here because you have just created a new universe, you probably want\n"
		"to create new Zone with\n"
		"\n"
		"  @CREATE /ZONE somename\n"
		"\n"
		"and then you can @TELEPORT yourself to it to start building.\n"
	));
	flags = json_object();
	json_object_set(flags, "system", json_true());
	json_object_set(flags, "fixed", json_true());
	json_object_set(flags, "hidden", json_true());
	json_object_set_new(_obj, "flags", flags);
	json_object_set_new(_obj, "contents", json_array());
	updateName();
	updateFlags();
	updateIdent();
}

Thing::ID
Limbo::id(void) const
{
	return ID_LIMBO;
}

Container *
Limbo::location(void) const
{
	return NULL;
}
