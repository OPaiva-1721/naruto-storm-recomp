# Sourced by the naruto_* scripts. Override any of these from the environment.
#   PS3_MAIN     main checkout (ROM, hdd0 live there; Codex worktrees read them, never write)
#   NARUTO_BUILD directory holding NarutoStorm.exe (default: build/naruto-win of the checkout running the script)
#   NARUTO_OUT   where logs, screenshots and pad.txt go (default: build/run of that checkout)
_here=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
PS3_MAIN=${PS3_MAIN:-$_here}
NARUTO_BUILD=${NARUTO_BUILD:-$_here/build/naruto-win}
NARUTO_OUT=${NARUTO_OUT:-$_here/build/run}
mkdir -p "$NARUTO_OUT"
