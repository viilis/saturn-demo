# Demo / hobby project for experimenting Sega Saturn game development

Built on [Jo Engine](https://jo-engine.org/) (MIT, see `LICENSE-jo-engine`), trimmed down to a Linux-only setup.

## Layout
| Path | What |
|---|---|
| `game/` | The game: `main.c`, `makefile`, assets in `cd/` |
| `jo_engine/` | Engine source code |
| `Compiler/LINUX/` | SH-2 toolchain (gcc 8.2.0) + newlib 3.3.0 headers |
| `Compiler/COMMON/` | Shared makefile and linker scripts |
| `docs/` | Doxygen config, 3D guide |

## Setup
Sega's SGL library and `IP.BIN` boot header are not included. Copy them from upstream [Jo Engine](https://github.com/johannes-fetz/joengine):
- `Compiler/COMMON/SGL_302j/`
- `Compiler/COMMON/IP.BIN`

## Build
```sh
cd game
./compile.sh
```

Output: `game.iso` + `game.cue`, run in a Saturn emulator.

## Editor (VS Code)
IntelliSense reads flags from `.vscode/compile_commands.json`. Regenerate after changing the makefile:

```sh
cd game
make clean && bear --output ../.vscode/compile_commands.json -- make
```

## Changes from upstream Jo Engine
- Windows and MacOS toolchains removed.
- Samples removed.
- newlib headers moved to `Compiler/LINUX/sh-none-elf/include`, makefile path fixed to match.

## Licensing
Own code: all rights reserved, no AI training (`LICENSE`). Third-party components: see `THIRD_PARTY_NOTICES.md`.
