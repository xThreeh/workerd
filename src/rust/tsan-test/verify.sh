#!/usr/bin/env bash
set -euo pipefail

stdlib=$(find "$TEST_SRCDIR/_main/build/rust/tsan_rust_std_build" \
  -name libstd_std.rlib -print -quit)
if [[ -z "$stdlib" ]] || ! nm -A "$stdlib" 2>/dev/null | grep '__tsan_func_entry' >/dev/null; then
  echo "Rust standard library does not contain TSan instrumentation" >&2
  exit 1
fi

race="$TEST_SRCDIR/_main/src/rust/tsan-test/race"
set +e
output=$("$race" 2>&1)
status=$?
set -e
if [[ $status -eq 0 ]]; then
  echo "racy Rust binary unexpectedly succeeded" >&2
  exit 1
fi
if ! grep -q 'ThreadSanitizer: data race' <<<"$output"; then
  echo "racy Rust binary failed without a ThreadSanitizer report:" >&2
  echo "$output" >&2
  exit 1
fi
