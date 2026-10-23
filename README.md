# Demo / hobby project for experimenting Sega Saturn game development

Built on [Jo Engine](https://jo-engine.org/) (MIT, see `engine/LICENSE-jo-engine`), trimmed down to a Linux-only setup.

## Layout
| Path | What |
|---|---|
| `game/` | The game: `main.c`, `makefile`, assets in `cd/` |
| `engine/` | Upstream Jo Engine tree |
| `engine/jo_engine/` | Engine source code |
| `engine/Compiler/LINUX/` | SH-2 toolchain (gcc 8.2.0) + newlib 3.3.0 headers |
| `engine/Compiler/COMMON/` | Shared makefile and linker scripts |
| `engine/docs/` | Doxygen config, 3D guide |

## Setup
Sega's SGL library and `IP.BIN` boot header are not included. Copy them from upstream [Jo Engine](https://github.com/johannes-fetz/joengine):
- `engine/Compiler/COMMON/SGL_302j/`
- `engine/Compiler/COMMON/IP.BIN`

## Build
```sh
cd game
./compile.sh
```

Output: `game.iso` + `game.cue`, run in a [Saturn emulator](https://github.com/ymir-emu/Ymir).

## Editor (VS Code)
IntelliSense reads flags from `.vscode/compile_commands.json`. Regenerate after changing the makefile:

```sh
cd game
make clean && bear --output ../.vscode/compile_commands.json -- make
```

## Changes from upstream Jo Engine
- Windows and MacOS toolchains removed.
- Samples removed.
- newlib headers moved to `engine/Compiler/LINUX/sh-none-elf/include`, makefile path fixed to match.

## Licensing
Own code: all rights reserved, no AI training (`LICENSE`). Third-party components: see `THIRD_PARTY_NOTICES.md`.
