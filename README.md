# Bonsai Time (Pebble)

A pixel bonsai watchface. Your tree grows new leaves as you accumulate daily steps, blossoms when you hit your goal, and sheds back to a bare branch every night at midnight.

Art by [PixelLab](https://pixellab.ai). Settings via **Clay** (`@rebble/clay`).

## Growth

| Stage | Progress toward daily goal |
| --- | --- |
| Bare | 0 steps (and every midnight) |
| Buds | &lt; 15% |
| Sparse | &lt; 35% |
| Leafy | &lt; 60% |
| Lush | &lt; 100% |
| Blossom | ≥ goal |

Default goal is **8,000** steps (adjustable in Settings). Watches without step data show **No steps** (or enable **Demo mode** to cycle stages).

## Settings (phone)

Open **Bonsai Time → Settings** in the Pebble/Rebble app:

- Hour format / leading zero / date
- Daily step goal
- Show steps + growth stage label
- Background: Zen garden · Dawn mist · Ink wash
- Demo mode

## Build

On Windows (Podman or Docker):

```powershell
./build.ps1
```

Output: `build/BonsaiTime.pbw`

```powershell
./build.ps1 -Phone 192.168.1.23
./build.ps1 -Zip
```

## CloudPebble

Import `dist/Bonsai-Time-cloudpebble.zip` into CloudPebble (or rebuild it with
`python build-env/make_zip.py`).

## Appstore

Listing copy and screenshot notes: [`STORE.md`](STORE.md).

```powershell
python tools/generate_store_screenshots.py
```

Output: `store/screenshots/{basalt,diorite,flint,emery}/` at exact Rebble sizes.

## Layout

```
src/c/              watchface, steps → growth mapping
src/pkjs/           Clay settings
resources/images/   PixelLab bonsai sprites + menu icon
```
