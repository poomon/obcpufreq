obcpufreq
=========

what
----
obcpufreq is a tiny program to show/set cpu frequencies via a pipemenu
of openbox3.

license
-------
[GPLv2] because most code are ripped from cpufreq-info which is under GPLv2.

dependencies
------------
* [libcpufreq][1] - It might be bundled with [cpufrequtils][1] package (or its successor cpupowerutils).
* (optional) `cpufreq-set` and `sudo` command are required to modify current cpufreq settings.

compile
-------
Make sure you have a header file of libcpufreq, e.g. `/usr/include/cpufreq.h`

    make

install
-------
Put a shiny new executable `obcpufreq` into somewhere you like.

settings
--------
It's quite same as other pipe menus.

1. Insert the following xml line within the root menu element `<menu>...</menu>` in
   `~/.config/openbox/menu.xml`.
   Don't forget to replace `/PATH/TO/obcpufreq` of the `execute` attribute with actual path of
   installed `obcpufreq`. you can change `id="" label=""` part as you wish.

       <menu id="cpufreq-menu" label="cpu freqs" execute="/PATH/TO/obcpufreq"/>

2. If you wish to change cpufreq policy by this menu it will be necessary to allow `sudo` command to
   invoke `cpufreq-set` command *without asking password*.
   You can adjust `cpufreq-set` command line by `CPUFREQ_SET` macro in `obcpufreq.c` and recompile.


[GPLv2]: http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
[1]: http://www.kernel.org/pub/linux/utils/kernel/cpufreq/cpufrequtils.html
