#/bin/bash
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

if [ "$1" = "vga" ] || [ "$1" = "text" ]; then
    make MODE="$1" all
else
    echo 'Please enter either "vga" or "text" as an argument to build.sh.'
fi