# imissu-lyrics

An animated visualizer of *"Merry Christmas, I Miss You"* by **Ghost**, featuring a falling snow effect and lyrics synchronized to the music. Available in two versions: a terminal-based renderer written in Python and a graphical 60 FPS renderer built with C++ and [raylib](https://www.raylib.com).

## Requirements

**Python version** (`imissu.py`)

- Python 3.x (standard library only — no external dependencies)

**C++ version** (`imissu_ray.cpp`)

- A C++17 compiler (e.g. `g++`)
- [raylib](https://www.raylib.com) (on Arch Linux: `sudo pacman -S raylib`)
- The song file at `~/Music/imissu.mp3` (path can be changed via `MUSIC_PATH` in the source)

## Usage

### Python

```bash
python imissu.py
```

Press `Ctrl+C` to exit.

### C++

Build and run:

```bash
g++ -O2 -std=c++17 imissu_ray.cpp -o imissu_ray $(pkg-config --cflags --libs raylib)
./imissu_ray
```

Controls:

| Key | Action |
|---|---|
| `← / →` | Seek ±3 seconds |
| `R` | Restart song |
| `D` | Toggle HUD |
| `ESC` | Quit |

## How it works

The snow is rendered as parallax particles in three depth layers with sinusoidal sway. Lyrics fade in and out following the playback position (`GetMusicTimePlayed`). The terminal version uses a character grid with ANSI escape codes instead.

## Credits

Song: *Merry Christmas, I Miss You* by Ghost
