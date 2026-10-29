#!/usr/bin/env bash
# to_street.sh -- with run.sh started, go boot -> Ultimate Mission -> skip the Ichiraku cutscene -> street gameplay,
# then walk forward with W (key.ps1). ~7 min. Saves street.png. Cutscene length varies: check street.png.
D=$(dirname "$0"); N="bash $D/nav.sh"
bash $D/to_village.sh s >/dev/null 2>&1
$N x 40 >/dev/null
for i in $(seq 1 40); do $N x 5 0x4000 >/dev/null; done
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "$(cygpath -w $D/key.ps1)" -Key W -Seconds 6 >/dev/null 2>&1
$N street 2 >/dev/null
