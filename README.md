# libui-ng: a portable GUI library for C

Fork of [andlabs/libui](https://github.com/andlabs/libui). This README is being written.<br>
[![Build Status, GitHub Actions](https://github.com/libui-ng/libui-ng/actions/workflows/build.yml/badge.svg)](https://github.com/libui-ng/libui-ng/actions/workflows/build.yml)

## About

Simple and portable (but not inflexible) GUI library in C that uses the native GUI technologies of each platform it supports.

## Status

libui-ng is currently **mid-alpha** software.

See [CHANGELOG.md](CHANGELOG.md)

*Old announcements can be found in the [news.md](old/news.md) file.*

## Runtime Requirements

* Windows: Windows Vista SP2 with Platform Update or newer
* Unix: GTK+ 3.10 or newer
* Mac OS X: OS X 10.8 or newer

## Build Requirements

* All platforms:
	* [Meson](https://mesonbuild.com/) 0.48.0 or newer
	* Any of Meson's backends; this section assumes you are using [Ninja](https://ninja-build.org/), but there is no reason the other backends shouldn't work.
* Windows: either
	* Microsoft Visual Studio 2013 or newer (2013 is needed for `va_copy()`) — you can build either a static or a shared library
	* MinGW-w64 (other flavors of MinGW may not work) — **you can only build a static library**; shared library support will be re-added once the following features come in:
		* [Isolation awareness](https://msdn.microsoft.com/en-us/library/aa375197%28v=vs.85%29.aspx), which is how you get themed controls from a DLL without needing a manifest
* Unix: nothing else specific
* Mac OS X: nothing else specific, so long as you can build Cocoa programs

## Building

libui uses only [the standard Meson build options](https://mesonbuild.com/Builtin-options.html), so a libui build can be set up just like any other:

```
$ # you must be in the top-level libui directory, otherwise this won't work
$ meson setup build [options]
$ ninja -C build
```

Once this completes, everything will be under `build/meson-out/`. (Note that unlike the previous build processes, everything is built by default, including tests and examples.)

The most important options are:

* `--buildtype=(debug|release|...)` controls the type of build made; the default is `debug`. For a full list of valid values, consult [the Meson documentation](https://mesonbuild.com/Running-Meson.html).
* `--default-library=(shared|static)` controls whether libui is built as a shared library or a static library; the default is `shared`. You currently cannot specify `both`, as the build process changes depending on the target type (though I am willing to look into changing things if at all possible).
* `-Db_sanitize=which` allows enabling the chosen [sanitizer](https://github.com/google/sanitizers) on a system that supports sanitizers. The list of supported values is in [the Meson documentation](https://mesonbuild.com/Builtin-options.html#base-options).
* `--backend=backend` allows using the specified `backend` for builds instead of `ninja` (the default). A list of supported values is in [the Meson documentation](https://mesonbuild.com/Builtin-options.html#universal-options).

Most other built-in options will work, though keep in mind there are a handful of options that cannot be overridden because libui depends on them holding a specific value; if you do override these, though, libui will warn you when you run `meson`.

The Meson website and documentation has more in-depth usage instructions.

For the sake of completeness, I should note that the default value of `--layout` is `flat`, not the usual `mirror`. This is done both to make creating the release archives easier as well as to reduce the chance that shared library builds will fail to start on Windows because the DLL is in another directory. You can always specify this manually if you want.

Backends other than `ninja` should work, but are untested by me.

## Installation

Meson also supports installing from source; if you use Ninja, just do

```
$ ninja -C build install
```

When running `meson`, the `--prefix` option will set the installation prefix. [The Meson documentation](https://mesonbuild.com/Builtin-options.html#universal-options) has more information, and even lists more fine-grained options that you can use to control the installation.

#### Arch Linux

Can be built from AUR: https://aur.archlinux.org/packages/libui-git/

## Documentation

Needs to be written. Consult `ui.h` and the examples for details for now.

## Language Bindings

libui was originally written as part of my [package ui for Go](https://github.com/andlabs/ui). Now that libui is separate, package ui has become a binding to libui. As such, package ui is the only official binding.

Other people have made bindings to other languages:

Language | Bindings
--- | ---
C++ | [libui-cpp](https://github.com/billyquith/libui-cpp), [cpp-libui-qtlike](https://github.com/aoloe/cpp-libui-qtlike)
C# / .NET Framework | [LibUI.Binding](https://github.com/NattyNarwhal/LibUI.Binding)
C# / .NET Core | [DevZH.UI](https://github.com/noliar/DevZH.UI), [SharpUI](https://github.com/benpye/sharpui/), [TCD.UI](https://github.com/tacdevel/tcdfx)
CHICKEN Scheme | [wasamasa/libui](https://github.com/wasamasa/libui)
Common Lisp | [jinwoo/cl-ui](https://github.com/jinwoo/cl-ui)
Crystal | [libui.cr](https://github.com/Fusion/libui.cr), [hedron](https://github.com/Qwerp-Derp/hedron)
D | [DerelictLibui (flat API)](https://github.com/Extrawurst/DerelictLibui), [libuid (object-oriented)](https://github.com/mogud/libuid)
Euphoria | [libui-euphoria](https://github.com/ghaberek/libui-euphoria)
Harbour | [hbui](https://github.com/rjopek/hbui)
Haskell | [haskell-libui](https://github.com/beijaflor-io/haskell-libui)
JavaScript/Node.js | [libui-node](https://github.com/parro-it/libui-node), [libui.js (merged into libui-node?)](https://github.com/mavenave/libui.js), [proton-native](https://github.com/kusti8/proton-native), [vuido](https://github.com/mimecorg/vuido)
Julia | [Libui.jl](https://github.com/joa-quim/Libui.jl)
Kotlin | [kotlin-libui](https://github.com/msink/kotlin-libui)
Lua | [libuilua](https://github.com/zevv/libuilua), [libui-lua](https://github.com/mdombroski/libui-lua), [lui](http://tset.de/lui/index.html), [lui](https://github.com/zhaozg/lui)
Nim | [ui](https://github.com/nim-lang/ui)
Perl6 | [perl6-libui](https://github.com/Garland-g/perl6-libui)
PHP | [ui](https://github.com/krakjoe/ui)
Python | [pylibui](https://github.com/joaoventura/pylibui)
Ruby | [libui-ruby](https://github.com/jamescook/libui-ruby), [LibUI](https://github.com/kojix2/libui), [Glimmer DSL for LibUI](https://github.com/AndyObtiva/glimmer-dsl-libui)
Rust | [libui-rs](https://github.com/rust-native-ui/libui-rs)
Scala | [scalaui](https://github.com/lolgab/scalaui)
Swift | [libui-swift](https://github.com/sclukey/libui-swift)

## Frequently Asked Questions

### Why does my program start in the background on OS X if I run from the command line?
OS X normally does not start program executables directly; instead, it uses [Launch Services](https://developer.apple.com/reference/coreservices/1658613-launch_services?language=objc) to coordinate the launching of the program between the various parts of the system and the loading of info from an .app bundle. One of these coordination tasks is responsible for bringing a newly launched app into the foreground. This is called "activation".

When you run a binary directly from the Terminal, however, you are running it directly, not through Launch Services. Therefore, the program starts in the background, because no one told it to activate! Now, it turns out [there is an API](https://developer.apple.com/reference/appkit/nsapplication/1428468-activateignoringotherapps) that we can use to force our app to be activated. But if we use it, then we'd be trampling over Launch Services, which already knows whether it should activate or not. Therefore, libui does not step over Launch Services, at the cost of requiring an extra user step if running directly from the command line.

See also [this](https://github.com/andlabs/libui/pull/20#issuecomment-211381971) and [this](http://stackoverflow.com/questions/25318524/what-exactly-should-i-pass-to-nsapp-activateignoringotherapps-to-get-my-appl).

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md)

## Screenshots

From examples/controlgallery:

![Windows](examples/controlgallery/windows.png)

![Unix](examples/controlgallery/unix.png)

![OS X](examples/controlgallery/darwin.png)
