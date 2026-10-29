#!/usr/bin/env bash
# to_village.sh [prefix] -- with run.sh already started, drive boot -> New Game -> Ultimate Mission -> village load.
# Fixed timings (measured 19/10 at ~30 fps in menus). Saves <prefix>_mode.png and <prefix>_load.png; the village
# itself (Ichiraku cutscene) shows ~60-110 s after the load shot: take more shots with nav.sh to check.
N="bash $(dirname "$0")/nav.sh"; p=${1:-v}
$N x 45 >/dev/null               # boot + opening movie
$N x 5 0x0008 >/dev/null         # Start -> title menu
$N x 2 0x0010 8 >/dev/null       # Up -> New Game
$N x 7 0x4000 >/dev/null         # Cross -> Mode Select
$N ${p}_mode 1 >/dev/null
$N x 15 0x4000 >/dev/null        # Cross -> Ultimate Mission Mode, story movie starts
$N x 3 0x0008 >/dev/null         # Start -> skip prompt
$N x 2 0x0010 8 >/dev/null       # Up -> YES
$N ${p}_load 4 0x4000            # Cross -> loading
