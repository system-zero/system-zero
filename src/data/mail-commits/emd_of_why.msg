This is the only one commit so far, for just this  commit message itself.

This is to mention a couple of things about the development procedure and
how we reached at this current point.

But first and speaking about the actual usage of the git development tracker
from our side.

With just one word: It Was awful! And of course it is not recommended. IMHO
it should punished by exercising a violent violence as soon is observated.
At the very least it can be provided as a "not to do it like this" example.

if for no other reason - which there are more than this one - just because
the usual commited diff, were usually more than a thousand lines length, but
most (especially at the beginning of this project) even much more than 1000.

Speaking for the beginning: the situationism drove me tonight, to look back in
the git history, and i found a lot of bugs in the commit messages themselves.
I wish to was possible somehow to edit those messages, with a reliable way, that
wouldn't introduce syncronization issues with other repositories.

Most of them were spelling mistakes, a couple of wrong information about
syntaxes and semantics, some serious ommisions, a couple of assumptions
for the commit puspose (ommisions of touched functions or units) and
many badly choosen subjects and finally: long commit messages (like this one
coincidently).

Apologizing a bit and by providing some excuses, i have to objectivelly testify.

This project was really about recording the development from the very
scratch point up to the completion, and for a reason: I believe that
there is some value (even to its code mistakes). If we could travel back in
eighties, it could be used extensivelly by some of our legendary hackers, or
or as (and here is the real reason) educational tool to explain at once many
computer concepts. It does that because it is a Declaration Type of Development,
one that its purpose is to declare intentions. Then the will, wisdom and time will
do its duty to the evolution. In this case its:
  - what is the minimum code that could get a system to be and feel (in a biderectionally
    way) useful and usable (both for the client and the computer system we live)

  - what is the minumum documentation that it could be enough for a human to  realize
    the machine and then: what it needs to be done, to have some work done

Then we stabilize our concepts and semantics and develop the code by providing
a test framework or functionalize the code by providing tiny envrironments that
do one thing without even have a sence that there even an outside.

This last realization it will probably help us to separate the concerns, describing
first this external environment that could influence our inner code with unhandled
conditions (in most cases in this primitive system is I/O  operations) or in a later
phase data from an external resource (such internet data). So in this case :

  - our system made by small systems, that get their arguments and do their job, by
    sanitizing first the arguments (usually at the very first code (i really like
    to just cut conditions at the very very first code and then slowly end up to a
    state that i know, that from this moment and now nothing can be wrong and with
    predictability

  - I/O operations

  - network  (specifications of data and so data handlers)

Okey to be fair: I do not know if the code could be in such state that could satisfy
Bill Joy or dmr: obviously not as there is a serious lack of education (without this
ever becomes an excuse (i do the best i can)).

But it would probably help me tremendously to explain quite a lot of things, and in
quite different areas of time, to curious humans beings thirsty to realize.
Okey again to be fair, personally donot have the qualifiers, but which might though
be enough for those beings around the Annapurna Hills or against the Hill that you
staring the very first golden sunlight at the fish tail of the Holy Machapuchare. 

So its enough to understand that this alone is an excuse, as the commit messages was
a way to describe concepts mostly, whereas the code itself can be divided in many some
sub-commits with the touched functionality described in sub-commit-messages. Again i
could wish to be a git future one day (if it is not already possible). Something like
a repository inside a repository.
But ofcourse even today can be analyzed by parsing `git log --patch`. Anyway.

And to be fair againg: this was always a personal style in SLang at the old days, or in
in those (two-three) C last years, so yes it is merely an excuse.

But the really reason about this commit was just because i found a very annoyed bug.

You see i really like to give attributes and you should do the same if you ask me.
If for no other reason - and there there are many - just to feel important!
To feel a bit of special! Because you have to know that i'm just a poor old kid from
the valley (no electicity and cars (real magic f life) so:

I met and with some became some kind of friends, some respectful people around the
Unix eartch, like uni professors, but also many very really really nice guys (many
of them from the legendary LFS days like Ken, but also Bruce and Randy and Dan and
Alexander but really too many to mention).  Still follow our lists as i first feel
like as an LFS another guy that was blessed to live in a great time with intensive
passion. This is always the preffered main fuel required for the development. In our
case was always to provide the minimum enough information to build a system from the
(almost) scratch point. Just made by human beings that they just love what they do.

Also for a couple of years in the vim mailing list (some of the best times also), or
around the olympic games at gentoo forums (really funny times :)). I remember also
the Sean from down-under and the Musca best every fullscreen X window manager -
better than ratpoison (as it came after five years of the legendary original code) -
so i wrote the man page there modelled by the mutt man page. Also speaking for mutt
as i still follow their development since the days i was the one that updated the mutt
page in BLFS. As a funny story, it also happened one mail of mine to be the main cause
to bring down a major site and i had taken a serious warning about that. Anyway!
It was just a bit unjustified, as it was actually a lot of a funny thing, which it
wasn't that funny, because it was about the mailing lists and moderns web like horrible
mail clients that do not respect gained by conscince sacred standards.
Fot the record I still insist that the mailing lists is the one of the best development
tools ever and there is no reason to adopt fooliness when we are really felt and used
their power.

But the situationism brought me once in an exchange with Bruno Haible
and Paul Eggert, with an admirable cooperation with two hall of famers
in the computing history.

So the commit about idx_t, the attribution actually should go mainly to
Paul Eggert (i should fix this with a way), and that  was actually the
reason that spawned this reaction (a serious bug in the commit history),
and the responsible reason for this (probably the longest ever commit
message ever so far) (of which i'm not that proud to be absolute honest).

Oh! and of course is Jason my friend behind Dictu (some of my best of
my time also), and SLang and our exchanges with John (I owe too too much
to him (i do not dare to even try to mention them here)).

But also with many others. Too many to mention them all here, but with
all equal great communication or|and cooperation. And with some, some
personal errors by my side. In those (not that many to be honest, but
still feel much), I wish I could go back in time to handle them differently.

The most memorized of this it was when i told to my friend Randy, my
boss at BLFS, that it was stupid habit to use sudo without a password,
and that it was done in public. I still think that is stupid though,
though there are better ways to tell the same thing or do not try to
impose it to others. Anyway i also do have a door open with a way to
do some things that requiring authorization. But yes when you are on
a multi environment, it is not so wise to leave your computer to do a
couple of things, without requiring a password with sudo.

But that's the way it is. If you press 'send' in mutt, the email doesn't come
back to you and you can not change a thing. So yes i really became fool in a
couple of occations in the public, but what to do! You can not actually hide
yourself in the UNIX mailing lists!!!

Happy to met you all guys in these interesting times to be.

This post will be saved for possible extensions.

This commit removes some self references in the Makefiles as we can do now
better than this. This is funny as most and most of my time i do coding for
personal things that they will never be public as they don't make sence to
be public.

As a resume. We declared our devoted time to 18 months to the completion.
This is not so far. We are just an init to do, which it isn't that simple
though at same time it is. We also want to use internally git with libgit2
and probably a C compiler (for the start tinycc can be enough (though at
some point also mir should be considered, or|and the static zig binary as
our compilers)).

But i feel that some time should be devoted to documentation which is always
too much of a hassle for my limited free time, as i lack compactness and correct
expressions in English Language (we could say a language into the Ascii range)
used mostly by programmers. 

Other than this, i feel that i see myself involved mostly on other things and
the code may it evolve in a more, lets say lazy way than usual. But noone knows.
