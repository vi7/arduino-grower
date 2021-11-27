#!/usr/bin/env sh

grep '^#define VERSION \".*\"$' include/version.h | cut -d' ' -f3 | tr -d '"'
