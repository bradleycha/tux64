# Tux64 Contribution Guide
<img src="../logo.png" width="156" height="156"/>

## Pull Requests

When submitting code, it is expected you are able to explain every line of code
you submit and _why_ you want your code accepted.  If it's not either obvious
from context what your code does or you provide no information, your pull
request will likely be rejected.

All pull requests should merge into the `master` branch.  Never submit pull
requests targetting a final release branch.

It is also expected to have a clean commit history.  Commits should be formatted
so that each commit does "one thing" and only does what it says it does.  You
should also make sure each commit message includes the subsystem it affects.

An example of a **bad** commit history looks like this:

```
fix bug
fix bad formtating
(no commit message)
aaaaaaaaa
fix regression and add new helper function
```

An example of a **good** commit history looks like this:

```
tux64-boot: fix incorrect virtual to physical address translation in tux64_boot_stage1_video_swap_buffers()
tux64-lib: fix typo in field for Tux64ArgumentsParseContext
tux64-lib: create new API for parsing arguments from a single-line string
tux64-boot: fix regression in stage-0 after incorrectly reallocating registers
```

Be prepared for changes to be requested and for you to explain yourself.  This
doesn't mean you're a bad programmer and should bugger off necessarily, as you
could get your code submitted after a few simple changes, but you shouldn't
expect your code to always be graciously accepted immediately.  As of writing
(October 2025), I'm in my Senior year of a CS and Math major and I'm also taking
graduate CS classes, and I also have other projects I work on.  If I take some
time to review your pull request, please be patient.  I'm not deliberately
ignoring you, I'm probably just busy or burnt out.

## Issues

Issues follow a similar principle to pull requests, but instead of explaining
your code, you should explain your issue if it's not obvious from the title.
You should also prefix the title of your issue with the relevant subsystem.
You should also provide enough information about your setup so that others can
reasonably recreate your issue (so that it can be debugged!).  If you're using
a modified `buildconf.sh` or `configure` command, that should be included.

Here is an example of a **bad** issue that will be deleted:

```
WHY DOENS'T THIS WORK

    Hye i tried running this on my laptop using an emulator and it doesnt' work!
    
    hEre is a link to a screenshot: (dead link to some random image hosting website)
```

Here is an example of a **good** issue that will get your problem solved:

```
tux64-boot: boot failure on PAL consoles

    When running tux64-boot-0.1.0, on a PAL console with a black screen.
    Monitoring the SysAD bus and status codes give status code STAGE0:4

    My buildconf.sh is the default, and I'm also using default configure
    commands.
```

