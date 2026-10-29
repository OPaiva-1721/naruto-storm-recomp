#!/usr/bin/env bash
# nav.sh <shot> [wait_s] [mask [frames]] -- optionally hold a pad button (cellPad mask, e.g. 0x4000 Cross,
# 0x0008 Start, 0x0010 Up), wait, then save $NARUTO_OUT/<shot>.png (PrintWindow, half size). Use "x" as a throwaway name.
. "$(dirname "$0")/env.sh"
name=$1; wt=${2:-3}; shift 2 2>/dev/null
[ -n "$1" ] && echo "$*" > "$NARUTO_OUT/pad.txt"
t=$(date +%s); until [ $(( $(date +%s) - t )) -ge $wt ]; do sleep 1; done
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "$(cygpath -w "$(dirname "$0")/shot.ps1")" -Out "$(cygpath -w "$NARUTO_OUT/$name.png")"
