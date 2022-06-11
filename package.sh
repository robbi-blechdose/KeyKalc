#!/bin/sh

make oclean
make TARGET=funkey

mkdir -p opk
cp main.elf opk/main.elf
cp icon.png opk/icon.png
cp keykalc.funkey-s.desktop opk/keykalc.funkey-s.desktop

mksquashfs ./opk KeyKalc.opk -all-root -noappend -no-exports -no-xattrs

rm -r opk