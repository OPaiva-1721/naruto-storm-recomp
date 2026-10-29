#!/usr/bin/env bash
# to_street_fast.sh -- keyboard route (naruto-pc profile) with run.sh already started: Esc skips the opening, Space = Load Game,
# Esc + Up + Space skips the story movie and then the Ichiraku cutscene, Space closes the instructions, W walks. ~3 min.
# Saves street.png in $NARUTO_OUT. Timings were measured 25/10; check street.png.
D=$(dirname "$0"); . "$D/env.sh"
K="powershell.exe -NoProfile -ExecutionPolicy Bypass -File $(cygpath -w "$D/key.ps1")"
sleep 40
$K -Key ESC -Seconds 0.3 >/dev/null; sleep 6          # skip opening -> title
$K -Key SPACE -Seconds 0.3 >/dev/null; sleep 5         # Load Game
$K -Key SPACE -Seconds 0.3 >/dev/null; sleep 25        # slot -> credits -> story movie
$K -Key ESC -Seconds 0.3 >/dev/null; sleep 3           # Skip? prompt
$K -Key UP -Seconds 0.2 >/dev/null; sleep 1
$K -Key SPACE -Seconds 0.3 >/dev/null; sleep 50        # YES -> load -> Ichiraku cutscene
$K -Key ESC -Seconds 0.3 >/dev/null; sleep 3
$K -Key UP -Seconds 0.2 >/dev/null; sleep 1
$K -Key SPACE -Seconds 0.3 >/dev/null; sleep 25        # skip cutscene -> street + instructions
for i in 1 2 3; do $K -Key SPACE -Seconds 0.3 >/dev/null; sleep 2; done
sleep 3
[ "${NARUTO_WALK:-1}" = 1 ] && $K -Key W -Seconds ${NARUTO_WALK_S:-6} >/dev/null   # NARUTO_WALK=0: measure at the fixed spawn point
bash "$D/nav.sh" street 2 >/dev/null
