#!/bin/bash
# Symbolize Evergreen and starboard stack traces from logcat output.
#
# Usage:
#   adb logcat -s starboard:* | ./symbolize_evergreen.sh <evergreen-libcobalt.so> [<starboard-libcobalt.so>]
#
# <evergreen-libcobalt.so>  Unstripped Cobalt 26 evergreen library.
#   Defaults to:
#   ~/git/cobalt-26/src/out/evergreen-arm-softfp_qa/lib.unstripped/libcobalt.so
#
# <starboard-libcobalt.so>  Unstripped Cobalt 25 Android starboard library.
#   Defaults to:
#   ~/git/cobalt-25/src/out/android-arm_qa/gradle/loader_app/build/app/intermediates/merged_native_libs/qa/out/lib/armeabi-v7a/libcobalt.so

set -e

EVERGREEN_LIB="${1:-$HOME/git/cobalt-26/src/out/evergreen-arm-softfp_qa/lib.unstripped/libcobalt.so}"
STARBOARD_LIB="${2:-$HOME/git/cobalt-25/src/out/android-arm_qa/gradle/loader_app/build/app/intermediates/merged_native_libs/qa/out/lib/armeabi-v7a/libcobalt.so}"

if [ ! -f "$EVERGREEN_LIB" ]; then
  echo "Warning: evergreen cobalt library not found: $EVERGREEN_LIB" >&2
  echo "Warning: evergreen+0x... frames will not be symbolized" >&2
  EVERGREEN_LIB=""
fi

if [ ! -f "$STARBOARD_LIB" ]; then
  echo "Warning: starboard library not found: $STARBOARD_LIB" >&2
  echo "Warning: libcobalt.so+0x... frames will not be symbolized" >&2
  STARBOARD_LIB=""
fi

# Find llvm-symbolizer
SYMB="/var/home/hferreiro/git/cobalt-26/src/third_party/llvm-build/Release+Asserts/bin/llvm-symbolizer"
if [ ! -x "$SYMB" ]; then
  SYMB="$(command -v llvm-symbolizer 2>/dev/null || true)"
fi
if [ -z "$SYMB" ]; then
  echo "Error: llvm-symbolizer not found" >&2
  exit 1
fi

symbolize() {
  local lib="$1"
  local offset="$2"
  local result
  result=$("$SYMB" --obj="$lib" "$offset" 2>/dev/null | head -2)
  local func loc
  func=$(echo "$result" | head -1)
  loc=$(echo "$result" | tail -1)
  echo "$func $loc"
}

while IFS= read -r line; do
  if [[ "$line" =~ evergreen\+0x([0-9a-fA-F]+) ]]; then
    sym=$(symbolize "$EVERGREEN_LIB" "0x${BASH_REMATCH[1]}")
    echo "$line  =>  $sym"
  elif [[ "$line" =~ libcobalt\.so\+0x([0-9a-fA-F]+) ]] && [ -n "$STARBOARD_LIB" ]; then
    sym=$(symbolize "$STARBOARD_LIB" "0x${BASH_REMATCH[1]}")
    echo "$line  =>  $sym"
  else
    echo "$line"
  fi
done
