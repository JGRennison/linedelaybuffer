#!/bin/bash

# This is loosely based on fdlinecombine's makedeb.sh
# https://github.com/vi/fdlinecombine/tree/master/deb

DIST=
VSUFFIX=-1
OUTDIR=packages
NONEWORIG=
DBUILDOPTS="-sa"

set -e

while getopts ":d:v:o:Snu" opt; do
  case $opt in
    d)
      DIST="$OPTARG"
      ;;
    v)
      VSUFFIX="$OPTARG"
      ;;
    o)
      OUTDIR="$OPTARG"
      ;;
    S)
      DBUILDOPTS+=" -S"
      ;;
    n)
      NONEWORIG=1
      ;;
    u)
      "$0" -S -d trusty -v -0~ppa1~trusty1
      "$0" -n -S -d saucy -v -0~ppa1~saucy1
      "$0" -n -S -d precise -v -0~ppa1~precise1
      "$0" -n -S -d quantal -v -0~ppa1~quantal1
      exit 0
      ;;
    ?)
      exit 1
      ;;
  esac
done

P=linedelaybuffer
VERSION="`cd ..  && make dumpversion`"
ORIGV="`sed -e 's/^v//; s/-/+/g' <<< "$VERSION"`"
V="${ORIGV}${VSUFFIX}"
FILES="linedelaybuffer.cpp makefile README.md COPYING-GPL.txt"

DIR="$OUTDIR/$P-$ORIGV"

rm -Rf "$DIR"

function cleanup() {
	rm -Rf "$DIR"
}
trap "cleanup" EXIT

mkdir -p "$DIR"
for i in $FILES; do
	cp -v ../"$i" "$DIR"/
done
echo "$VERSION" > "$DIR"/version

if [ -z "$NONEWORIG" ]; then
	tar -C "$OUTDIR" -czf "$OUTDIR"/"${P}_${ORIGV}.orig.tar.gz" "$P-$ORIGV"
fi

cp -R debian "$DIR"/
(
	cd "$DIR"
	dch --create --newversion $V --package "$P" --no-force-save-on-release --urgency low ${DIST:+ --distribution "$DIST"} "Package of revision: $VERSION"
	debuild $DBUILDOPTS
)
