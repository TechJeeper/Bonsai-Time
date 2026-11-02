# Rebble Appstore listing — Bonsai Time

Copy these fields into [dev-portal.rebble.io](https://dev-portal.rebble.io/submit).

| Field | Value |
| --- | --- |
| App name | Bonsai Time |
| Type | Watchface |
| Category | Faces *(automatic)* |
| Website | *(optional)* |
| Source code | https://github.com/TechJeeper/Bonsai-Time |
| `.pbw` | `build/BonsaiTime.pbw` |

## Description

Paste into each platform asset collection (max 1600 characters):

```
A living pixel bonsai on your wrist. Walk, watch leaves appear, hit your goal to blossom — then at midnight it sheds and starts again.

• Six PixelLab stages: Bare → Buds → Sparse → Leafy → Lush → Blossom
• Grows from today’s steps; resets every midnight
• Configurable daily step goal
• Optional date, step count, and stage label
• Three backgrounds: Zen, Dawn, Ink
• Demo mode for watches without step data
• Clay phone settings

Color platforms only (Pebble Time / Time 2). Best on step-capable watches.
```

## Short blurb (optional)

```
Pixel bonsai that grows with your daily steps, blossoms at your goal, and sheds at midnight.
```

## Release notes (v1.0.0)

```
Initial release: step-grown PixelLab bonsai with Clay settings, midnight shed, and blossom goal.
```

## Screenshots

Exact platform sizes (no watch bezel). Regenerate with:

```powershell
python tools/generate_store_screenshots.py
```

| Platform | Size | Folder |
| --- | --- | --- |
| basalt (Pebble Time) | 144×168 | `store/screenshots/basalt/` |
| emery (Pebble Time 2) | 200×228 | `store/screenshots/emery/` |

Five shots per platform:

1. Bare · Zen background  
2. Buds (+date)  
3. Leafy · Dawn mist  
4. Lush · Ink wash  
5. Blossom

## Listing checklist

- [ ] Screenshots uploaded per platform (up to 5)
- [ ] Description above on each asset collection
- [ ] Source URL set to the GitHub repo
- [ ] Publish `build/BonsaiTime.pbw` as release 1.0.0
