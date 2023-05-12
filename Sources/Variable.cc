#include "Nexus/Variable.hh"
#include "Nexus/Actor.hh"

using namespace Nexus;

const char *
Variable::textValue(void) const
{
	json_t *val;

	if((val = json_object_get(_obj, "text")))
	{
		return json_string_value(val);
	}
	if((val = json_object_get(_obj, "value")))
	{
		return json_string_value(val);
	}
	return NULL;
}

json_t *
Variable::value(void)
{
	return json_object_get(_obj, "value");
}

bool
Variable::setText(const char *text)
{
	json_object_del(_obj, "value");
	if(text)
	{
		json_object_set_new(_obj, "text", json_string(text));
	}
	else
	{
		json_object_del(_obj, "text");
	}
	markDirty();
	return true;
}

bool
Variable::setValue(json_t *json)
{
	json_object_del(_obj, "text");
	if(json)
	{
		json_object_set(_obj, "value", json);
	}
	else
	{
		json_object_del(_obj, "value");
	}
	markDirty();
	return true;
}

void
Variable::examineSections(Actor *player)
{
	const char *text;
	json_t *val;

	Thing::examineSections(player);
	if((text = textValue()))
	{
		player->send("-[ Text value ]--------------------------------------------------------\n");
		player->sendf("%s\n", text);
	}
	if((val = value()))
	{
		char *buf;

		player->send("-[ JSON value ]--------------------------------------------------------\n");
		buf = json_dumps(val, JSON_INDENT(2)|JSON_SORT_KEYS|JSON_ENSURE_ASCII);
		player->sendf("%s\n", buf);
		free(buf);
	}
}
