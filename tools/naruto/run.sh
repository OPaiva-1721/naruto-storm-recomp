#!/usr/bin/env bash
# run.sh <tag> [ENV=val ...] -- launch NarutoStorm.exe with closed-loop input (PAD_FILE=$NARUTO_OUT/pad.txt).
# Blocks until the game exits; run it in the background and drive it with nav.sh / to_village.sh.
# Log: $NARUTO_OUT/<tag>.log (noisy lines filtered). Stop with: powershell -c "Stop-Process -Name NarutoStorm -Force"
. "$(dirname "$0")/env.sh"
tag=$1; shift
: > "$NARUTO_OUT/pad.txt"
cd "$NARUTO_BUILD" || exit 1
env PS3_HDD0_ROOT="$(cygpath -m "$PS3_MAIN")/build/naruto-hdd0" PS3_VERBOSE=0 PS3_SAIL_AUTOSTART=1 \
    GCM_SEMA_ACQUIRE=1 PS3_NULL_READ=0 PAD_FILE="$(cygpath -w "$NARUTO_OUT/pad.txt")" "$@" \
    ./NarutoStorm.exe "$(cygpath -m "$PS3_MAIN")/rom/naruto-storm/game/PS3_GAME/USRDIR/EBOOT.elf" 2>&1 \
  | grep -av --line-buffered "drain-resume\|event_queue_receive\|semaphore_wait(sem=2\|null-read" > "$NARUTO_OUT/$tag.log"
