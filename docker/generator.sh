#!/bin/bash

TERRAMA2_TAG=$2
TERRAMA2_
version=2.0.0
fullVersion="$version-rc1"

sed -r \
    -e 's!%%VERSION%%!'"$version"'!g' \
    -e 's!%%FULL_VERSION%%!'"$fullVersion"'!g' \
    -e 's/^(FROM (debian|buildpack-deps|alpine)):.*/\1:'"$tag"'/' \
    "example.in" > "example"