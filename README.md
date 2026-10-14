# Demo / hobby project for experimenting Sega Saturn game development

Built on [Jo Engine](https://jo-engine.org/) (MIT, see `LICENSE`), trimmed down to a Linux-only setup.

## Layout
| Path | What |
|---|---|
| `Projects/*/` | The game: `main.c`, `makefile`, assets in `cd/` |
| `jo_engine/` | Engine source code |
| `Compiler/LINUX/` | SH-2 toolchain (gcc 8.2.0) + newlib 3.3.0 headers |
| `Compiler/COMMON/` | Shared makefile, Sega SGL library, `IP.BIN` boot header for CDs |
| `Samples/` | Jo Engine examples |

## Build
```sh
cd Projects/demo3D
./compile.sh
```

Output: `game.iso` + `game.cue`, run in a Saturn emulator.

## Editor (VS Code)
IntelliSense reads flags from `.vscode/compile_commands.json`. Regenerate after changing the makefile:

```sh
cd Projects/demo3D
make clean && bear --output ../../.vscode/compile_commands.json -- make
```

## Changes from upstream Jo Engine
- Windows and MacOS toolchains removed.
- newlib headers moved to `Compiler/LINUX/sh-none-elf/include`, makefile path fixed to match.

## Licensing
Engine is MIT, newlib is permissive (BSD-style). SGL and `IP.BIN` are Sega's code.
