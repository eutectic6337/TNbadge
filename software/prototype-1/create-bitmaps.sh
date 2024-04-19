#! /bin/sh
# needs ImageMagik https://imagemagick.org/script/download.php
header_comment='/* 104x212 bitmap 2762 bytes */'

resize='magick "$IN" -resize 212x208^ -crop 212x208+0+0 -monochrome "$TMP1" 2>$R.err'
top='magick "$TMP1" -crop 212x104 "pbm:${TOP}" 2> $S.err'
bot='magick "$TMP1" -chop 212x104 -crop 212x104 "pbm:${BOT}" 2> $S.err'
rotate="magick \"pbm:\$F\" -rotate -90 pbm:- |
	tail -n +3 |
	xxd -g 1 |
	awk '{for(i=0;i<16;i++){printf \"0x%02x, \", \$(2+i)};print \"\"}'"

PREFIX=$( basename "$0"|sed 's/\..*$//' )
TMP=/tmp/$PREFIX-$$
trap "rm -rf '$TMP'" 0
mkdir -p "$TMP"

[ -z "$1" ] && {
	echo "
Usage: `basename $0` <imagefilename>

image file should contain
	212x104 black/white bitmap tiled above
	212x104 red/white bitmap

larger image file will be scaled down

any colors present will be flattened to monochrome
"
	exit 1
}

IN="$1"
[ -f "$IN" ] || {
	echo "Input image file \"$IN\" not found"
	exit 2
}

TMP1="$TMP/image.png"
TOP="$TMP/top"
BOT="$TMP/bot"

R="$TMP/resize"
S="$TMP/split"

eval $resize &&
eval $top &&
eval $bot && {
	F="${TOP}" && eval $rotate > black.h
	F="${BOT}" && eval $rotate > red.h
}

