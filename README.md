# Hyphenate

Command line text hyphenation tool. Inserts Unicode soft-hyphens in text read from `stdin`, outputting to `stdout`.

The language argument is optional, if omitted the current user language is tried.

Usage: `cat text.txt | hyphenate [lang_LANG] > hyphenated.txt`

## Requirements

This package requires `libhyphen`. For Debian based distributions:

```bash
# Install the base library and any dictionaries you might need:
sudo apt install libhyphen hyphen-{en-us,en-gb,es,fr,pt-br,pt-pt,de,nl}
# And to build from source:
sudo apt install libhyphen-dev
```

## Building from source

```bash
# Fetch sources:
git clone $repository

# Create build environment and compile:
cd hyphenate/build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make

# Optionally run a quick test:
echo "What we've got here is a failure to communicate." | ./hyphenate en_US
#=> What we've got here is a fail&shy;ure to com&shy;mu&shy;ni&shy;cate.

# Install system-wide:
sudo make install
```

## Notes and license

This project is available on [GitHub](https://github.com/StefanHamminga/hyphenate).

The project is licensed as [GPLv2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html) and may be freely used, modified and distributed as such. The license file is included in the project directory.

Copyright 2016 [Stefan Hamminga](mailto:stefan@prjct.net) - [prjct.net](https://prjct.net)
