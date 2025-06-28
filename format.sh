find . -name '*.hpp' -o -name '*.cpp' -o -name '*.c' -o -name '*.h' | grep -v '^./extern/' | xargs clang-format -i
