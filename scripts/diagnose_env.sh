#!/usr/bin/env bash
set -euo pipefail

ROOT="$(pwd)"
echo "Repo root: $ROOT"
echo

echo "1) Locate bass.h (must exist):"
find "$ROOT" -type f -iname "bass.h" -print || echo "bass.h not found"

echo
echo "2) Show any project files that mention include dirs or language standard:"
grep -n -E "AdditionalIncludeDirectories|LanguageStandard|/std:" "$ROOT"/XNFSMusicPlayer* -R || true

echo
echo "3) Check vcpkg cpr installation (x86 must match Win32 build):"
if command -v vcpkg >/dev/null 2>&1; then
  vcpkg list | grep cpr || echo "cpr not installed via vcpkg (x86)"
else
  echo "vcpkg not in PATH; check C:/Users/<you>/vcpkg/installed/x86-windows/include/cpr"
  ls -la "$HOME/vcpkg/installed/x86-windows/include/cpr" 2>/dev/null || true
  ls -la "C:/Users/$(whoami)/vcpkg/installed/x86-windows/include/cpr" 2>/dev/null || true
fi

echo
echo "Paste the output here."