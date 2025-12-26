find . -name '*.hpp' -o -name '*.cpp' -o -name '*.c' -o -name '*.h' \
  | grep -v '^./extern/' \
  | xargs -P "$(nproc)" -I{} sh -c 'echo "Formatting {}"; clang-format -i "{}"'
