# Contributing to JL

Thanks for helping. JL is small on purpose: it is Prism Launcher, pointed at
Fabric. Most PRs are either a UI polish or a Fabric-specific behaviour fix.

## Getting started

```bash
git clone --recursive https://github.com/bufferclick/JLac
cmake --preset default
cmake --build --preset default
```

You need CMake 3.25+, Qt 6.8+, a C++23 compiler and
[vcpkg](https://github.com/microsoft/vcpkg) (the preset uses `cmake/vcpkg` as
its toolchain).

The Windows build used for releases runs the `Build JL for Windows` workflow,
which produces `JL.exe` and `JL-Windows-x64.zip`.

## Code style

All files are formatted with `clang-format` using `.clang-format`; run it on the
files you changed.

Conventions for C++:

- Types are `PascalCase`.
- Private/protected data members are `m_camelCase`; statics are `s_camelCase`.
- Public data members are `camelCase`.
- Static constants and macros are `SCREAMING_SNAKE_CASE`.
- Functions and non-`const` globals are `camelCase`.
- Enum constants are `PascalCase`.

Avoid inventing abbreviations. Prefer a descriptive name over a short one.

## UI rules

JL should feel like an application, not a skin:

- One accent, near-black surfaces, no gradients or glow.
- Never hard-code Prism Launcher branding; use `BuildConfig` values.
- Prefer existing widgets and pages over new ones. If you do add one, make it
  work in both the Fabric (`CustomPage`) and instance contexts.

## Pull requests

- Keep them small and describe *why*, not just *what*.
- Say which loader/instance paths you tested.
- Screenshots help for anything visual.

## Licence

JL is GPL-3.0. By contributing you agree your work is licensed the same way.
