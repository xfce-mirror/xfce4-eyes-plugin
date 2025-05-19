## What is it?

xfce4-eyes-plugin is a xfce4 panel plugin that adds eyes which watch
your every step. Scary!

Portions of the xfce4-eyes code were taken from geyes. These, and the rest of
the code are licensed under the terms of the GNU General public license; see
the [`COPYING`](COPYING) file.


## Usage

Simply add the plugin to the panel and select a theme in the
configuration dialog. Depending on how big the eyes are and where on
the panel you want the eyes to appear, you might find changing the
single-row option useful. It becomes effective only in multi-row mode.


## Installation

The file [`INSTALL`](INSTALL) contains generic installation instructions.


## How to report bugs?

Before reporting bugs or asking for new features, please review existing
issues at https://gitlab.xfce.org/panel-plugins/xfce4-eyes-plugin, because
your issue or request might have been reported already or be in planning.


## Easy build instructions

If you're interesting in building the plugin yourself, these
instructions provided here will work for most users. If not, please
look at the INSTALL file or ask at a forum for your linux distribution
or try the methods explained on https://www.xfce.org/community. Make
sure you have installed the needed dependencies (if you have installed
Xfce, you already have all required dependencies installed).

For the panel being able to find the plugin, it is important to set
the proper prefix. The prefix is the place in the filesystem where the
plugin files gets installed. It has to match the prefix used for
building the panel. There's nothing the plugin can do about that
requirement. When you're using the panel provided by the package
management system of your distribution, then the prefix is in most
cases /usr, otherwise the default prefix is /usr/local.

If you want to install the current version from git or a tarball
downloaded from https://archive.xfce.org/, execute the
following command in the plugin project directory:

```sh
meson setup build
```

If this fails, you should receive an error message telling you
the cause for the failure (e.g. missings libraries). If you're missing
a dependency you need to install it using the package management
system of your distribution. Distributions commonly have two versions
of a software package: One containing the supplementary files needed
for compiling other packages, and the other one providing the runtime
libraries etc. While the latter is usually installed, the former often
is not, so better check this.

Note: To solve distribution-specific problems the most efficient way
is to ask at a forum for your distribution, not on a general forum.

Then do:

```sh
meson compile -C build
```

If this fails, create an issue on
https://gitlab.xfce.org/panel-plugins/xfce4-eyes-plugin, or send a mail
to the xfce mailing list and provide make output.

Finally, and usually as root:

```sh
meson install -C build
```

Note: Depending on your prefix, this might overwrite an existing
version of the plugin.

You can later uninstall the plugin (as root) with:

```sh
meson uninstall
```

The panel should then recognize the new plugin, if it doesn't try to
restart it using `xfce4-panel -r`. If it still doesn't work after that
try to ask for help somewhere (forums, mailing lists, #xfce on
IRC). Please do not report such problems on the bug tracker.


## Translating the plugin for your language

If you need help getting started with translating the plugin into your
language, please visit this wiki page for a start:
https://wiki.xfce.org/translations/translation_guidance_in_xfce

TRANSLATORS, PLEASE MAKE SURE YOU CHECK YOUR FILE FOR ERRORS BEFORE
UPLOADING IT! Otherwise, it will break compilation of the plugin. It
is easy to do this with the following command (where file.po is the po
file of your language):

`msgfmt -c --check-accelerators=_ -v -o /dev/null <file.po>`
