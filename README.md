<p align="center">
  <img alt="JL" src="/program_info/jlac.svg" width="96" height="96">
</p>

<h1 align="center">JL</h1>

<p align="center">
  <b>Just Like any client.</b><br />
  A Fabric-only Minecraft launcher for Windows, built on Prism Launcher.
</p>

<p align="center">
  <a href="https://github.com/bufferclick/JLac">
    <img alt="View in GitHub" src="https://img.shields.io/badge/View%20in-GitHub-0B0B0C?style=for-the-badge&logo=github&logoColor=white">
  </a>
  <a href="https://github.com/bufferclick/JLac/releases">
    <img alt="Download" src="https://img.shields.io/badge/download-JL.exe-F2F2F3?style=for-the-badge&logoColor=0B0B0C">
  </a>
</p>

---

## What this is

JL takes Prism Launcher — the launcher most people already trust for managing
Minecraft installs — and points it at one job: **Fabric**.

Same instance format, same folder layout, same everything, because JL *is* a
Prism Launcher build. Only the name, the look and the loader choices are
different.

### v0.0.1

- **Fabric only.** No Forge, NeoForge, Quilt or LiteLoader in the loader pickers.
  Fabric is preselected and stays selected.
- **Import your Prism instances.** On first start JL offers to take over your
  existing Prism Launcher data folder (Windows: `%APPDATA%\PrismLauncher`), so
  every instance, world, mod and account carries over untouched.
- **Every Fabric update in one place.** *Help → Fabric Versions* lists every
  Fabric loader build grouped by Minecraft version, with the recommended build
  called out. Search it, filter by game version, refresh it.
- **Microsoft login** for online play, with device-code sign-in — no password
  ever touches the launcher.
- **A quiet default look.** Near-black surfaces, one accent, no clutter. Other
  themes are still in *Settings → Appearance* if you want them.
- **`JL.exe`**, built and published from this repository on every tag.

### Download

Grab the latest `JL.exe` (or the full portable `JL-Windows-x64.zip`) from the
[releases page](https://github.com/bufferclick/JLac/releases). Unzip anywhere and
run it — the portable zip keeps every instance, mod and setting next to the exe.

## Building

JL builds exactly like Prism Launcher does: CMake 3.25+, Qt 6.8+, a C++23
compiler and [vcpkg](https://github.com/microsoft/vcpkg).

```bash
git clone --recursive https://github.com/bufferclick/JLac
cmake --preset default
cmake --build --preset default
```

The Windows binary is produced by the `Build JL for Windows` workflow on every
`v*` tag and attached to the matching release.

## Credits

JL is a fork of [Prism Launcher](https://github.com/PrismLauncher/PrismLauncher),
which is a fork of PolyMC, which is a fork of MultiMC. All of their work is
shipped here under the GPL-3.0 — see the **Credits** tab in *About JL*. JL is
not endorsed by Prism Launcher, MultiMC or Mojang.
