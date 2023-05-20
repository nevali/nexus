#include <cstring>

#include "WARP/Flux/Diagnostics.hh"

#include "Guide.hh"

#include "Nexus/Actor.hh"

using namespace Nexus;
using namespace Nexus::Modules::Guide;
using namespace WARP::Flux::Diagnostics;

typedef struct
{
	const char *name;
	const char *text;
	const char *additional;
} HelpEntry;

bool
HELP::execute(Actor *actor)
{
	if(argc() < 2)
	{
		return intro(actor);
	}
	if(!strcasecmp(argv(1), "intro") || !strcasecmp(argv(1), "introduction") || !strcasecmp(argv(1), "contents"))
	{
		return intro(actor);
	}
	if(!strcasecmp(argv(1), "disclaim") || !strcasecmp(argv(1), "disclaimer") || !strcasecmp(argv(1), "disclaimers"))
	{
		return disclaimers(actor);
	}
	actor->sendf("Sorry, I can't find an Operator's Guide topic named '%s'\n", argv(1));
	return false;
}

bool
HELP::intro(Actor *actor)
{
	actor->send("=[ Operator's Guide ]==================================================\n");
	actor->send("                 I  N  T  R  O  D  U  C  T  I  O  N                    \n");
	actor->send("-----------------------------------------------------------------------\n");
	actor->send("  Dear Valued Customer,\n");
	actor->send("\n");
	actor->send("  Congratulations on your purchase of the greatest marvel humankind\n"
		"  has ever seen, certainly the finest wonder to grace your home, and\n"
		"  without a doubt the most affordable AND easy-to-use product of its\n"
		"  or indeed of any other kind! And, since 2023, guaranteed** free of\n"
		"  Gamma Rays, Exotic Particles, and Strange Matter!\n"
		"\n"
		"  What may appear at first glance to be a simple illuminated crystal\n"
		"  panel projecting strange messages from another realm, as you might\n"
		"  purchase from any vendor of occult trinkets or curios, is in fact\n"
		"  your gateway, or to be more accurate, a one-way ticket on a non-stop\n"
		"  express, to whole new universes--including those that have yet to\n"
		"  be imagined, and some that we can all agree ought not to be.\n"
		"\n"
		"  Of course, you know all of this. That's why you're reading this\n"
		"  guide now; the feverishly-discarded packaging of your pristine new\n"
		"  Doomsday(R) 9000(tm) Quantum Decelerator Plus(tm) strewn around you\n"
		"  in your excitement at the prospect of finally seeing those words for\n"
		"  yourself:\n"
		"\n"
		"     Greetings, Operator!\n"
		"\n"
		"  Well, the time is finally here. Was it everything you imagined? I'm\n"
		"  sure it was. In any event, it's time to grab your toolkit--you'll\n"
		"  find a quantum flux suppressor is essential (any well-known brand\n"
		"  will do), and make sure you don't accidentally throw away the multi-\n"
		"  tool included in the box; the last thing you want to do is find\n"
		"  yourself in an infinite-recursive-descent loop without a multi-tool\n"
		"  at hand--unless you really do want it to be the last thing you do,\n"
		"  of course, in which case, discard as you see fit!\n"
		"\n"
		"  And so the time has come for me to bid you farewell, and wish you\n"
		"  the very best of luck on your imminent adventures. With the\n"
		"  unrivalled, and frankly barely-imaginable, atomic power of the\n"
		"  Doomsday(R) 9000(tm) Quantum Decelerator Plus(tm) at your finger-\n"
		"  tips, you are quite literally unstoppable!!!\n"
		"\n"
		"  Yours unendingly,\n"
		"\n"
		"  Herbert J. Doomsday III\n"
		"  President & CEO\n"
		"  Doomsday Manufacturing Co., Ltd.\n"
		"\n"
		"  p.s., please see '@HELP disclaimers' for notices we are legally\n"
		"    required to include with this product -- a passcode will be\n"
		"    required.\n"
	);
	actor->send("=======================================================================\n");
	return true;
}

bool
HELP::disclaimers(Actor *actor)
{
	if(argc() >= 3)
	{
		actor->send("Sorry, that passcode is not correct. You will continue to be opted in to all features.\n"
			"Please note that no responsibility can be taken for death or inury as a\n"
			"result of features which you have chosen not to or failed to opt out of.\n");
		return false;
	}
	actor->send("In order to read the disclaimers and withdraw your consent from\n"
		"certain value-added features, such as implosion, quantum displacement,\n"
		"direct neural marketing, and Sudden Uncertainty Syndrome, you must\n"
		"enter the passcode printed on the 2cm x 1cm barcode label on the inner\n"
		"side of the opening flap of the Doomsday(R) 9000(tm) Quantum\n"
		"Decelerator Plus(tm)'s original packaging, and enter it as:\n"
		"\n"
		"  @HELP disclaimers <passcode>\n"
		"\n"
		"Otherwise, for your convenience, you will remain automatically opted in\n"
		"to all features.\n"
		);
	return false;
}
