#!/usr/bin/env python3
"""Render Rebble App Store screenshots at exact platform resolutions.

No watch bezel — store requirement. Showcases bonsai growth stages.
"""
from __future__ import annotations

import os
import shutil
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "store" / "screenshots"
BONSAI = ROOT / "resources" / "images" / "bonsai"

# Exact display sizes (Rebble store). color flag matches PBL_COLOR.
PLATFORMS = {
    "basalt": (144, 168, True),
    "diorite": (144, 168, False),
    "flint": (144, 168, False),
    "emery": (200, 228, True),
}

# Approximate Pebble GColor values used by bonsai.c backgrounds.
BRASS = (170, 119, 0)          # GColorBrass — Zen garden
PICTON_BLUE = (85, 170, 255)   # GColorPictonBlue — Dawn mist
BLACK = (0, 0, 0)              # Ink wash
WHITE = (255, 255, 255)

# Up to 5 store shots — growth arc from bare to blossom.
SCENES = [
    {
        "file": "01-bare.png",
        "sprite": "bare.png",
        "label": "BARE",
        "steps": "0 / 8000",
        "bg": BRASS,
        "fg": BLACK,
    },
    {
        "file": "02-buds.png",
        "sprite": "buds.png",
        "label": "BUDS",
        "steps": "920 / 8000",
        "bg": BRASS,
        "fg": BLACK,
        "show_date": True,
    },
    {
        "file": "03-leafy.png",
        "sprite": "leafy.png",
        "label": "LEAFY",
        "steps": "4100 / 8000",
        "bg": PICTON_BLUE,
        "fg": BLACK,
    },
    {
        "file": "04-lush.png",
        "sprite": "lush.png",
        "label": "LUSH",
        "steps": "6700 / 8000",
        "bg": BLACK,
        "fg": WHITE,
    },
    {
        "file": "05-blossom.png",
        "sprite": "blossom.png",
        "label": "BLOSSOM",
        "steps": "8120 / 8000",
        "bg": BRASS,
        "fg": BLACK,
    },
]


def font(size: int, bold: bool = False) -> ImageFont.ImageFont:
    if bold:
        candidates = [
            "C:/Windows/Fonts/consolab.ttf",
            "C:/Windows/Fonts/lucon.ttf",
            "C:/Windows/Fonts/segoeuib.ttf",
            "C:/Windows/Fonts/arialbd.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        ]
    else:
        candidates = [
            "C:/Windows/Fonts/segoeui.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/calibri.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        ]
    for path in candidates:
        if os.path.isfile(path):
            return ImageFont.truetype(path, size)
    return ImageFont.load_default()


def leco_font(size: int) -> ImageFont.ImageFont:
    candidates = [
        "C:/Windows/Fonts/consolab.ttf",
        "C:/Windows/Fonts/lucon.ttf",
        "C:/Windows/Fonts/courbd.ttf",
        "C:/Windows/Fonts/arialbd.ttf",
    ]
    for path in candidates:
        if os.path.isfile(path):
            return ImageFont.truetype(path, size)
    return font(size, bold=True)


def draw_text_center(draw: ImageDraw.ImageDraw, text: str, y: int, w: int, fnt, fill) -> None:
    bbox = draw.textbbox((0, 0), text, font=fnt)
    tw = bbox[2] - bbox[0]
    draw.text(((w - tw) // 2, y), text, font=fnt, fill=fill)


def to_bw_sprite(src: Image.Image) -> Image.Image:
    rgba = src.convert("RGBA")
    out = Image.new("RGBA", rgba.size, (0, 0, 0, 0))
    px = rgba.load()
    opx = out.load()
    w, h = rgba.size
    for y in range(h):
        for x in range(w):
            r, g, b, a = px[x, y]
            if a < 32:
                continue
            lum = 0.299 * r + 0.587 * g + 0.114 * b
            if lum < 70:
                opx[x, y] = (0, 0, 0, 255)
            else:
                opx[x, y] = (255, 255, 255, 255)
    return out


def load_sprite(name: str, color: bool, target: int) -> Image.Image:
    img = Image.open(BONSAI / name).convert("RGBA")
    if not color:
        img = to_bw_sprite(img)
    if img.size[0] != target:
        img = img.resize((target, target), Image.NEAREST)
    return img


def render(w: int, h: int, color: bool, scene: dict) -> Image.Image:
    bg = scene["bg"] if color else BLACK
    fg = scene["fg"] if color else WHITE
    img = Image.new("RGB", (w, h), bg)
    draw = ImageDraw.Draw(img)

    time_size = 36 if w >= 200 else 28
    draw_text_center(draw, "10:42", 2 if h > 180 else 0, w, leco_font(time_size), fg)

    if scene.get("show_date"):
        y_after_time = 40 if w >= 200 else 32
        draw_text_center(draw, "Sun 2 Nov", y_after_time, w, font(max(11, w // 13)), fg)

    sprite_size = 72 if w >= 200 else 56
    sprite = load_sprite(scene["sprite"], color, sprite_size)
    sprite_x = (w - sprite_size) // 2
    time_bottom = 64 if scene.get("show_date") else 48
    caption_h = 42
    band_top = time_bottom
    band_bot = h - caption_h
    sprite_y = band_top + (band_bot - band_top - sprite_size) // 2
    if sprite_y < band_top:
        sprite_y = band_top
    if sprite_y + sprite_size > band_bot:
        sprite_y = band_bot - sprite_size
    img.paste(sprite, (sprite_x, sprite_y), sprite)

    label_y = h - (48 if w >= 200 else 40)
    draw_text_center(draw, scene["label"], label_y, w, font(max(14, w // 11), bold=True), fg)
    draw_text_center(
        draw, scene["steps"], h - (22 if w >= 200 else 18), w, font(max(11, w // 13)), fg
    )

    return img


def main() -> None:
    if OUT.exists():
        shutil.rmtree(OUT)
    for plat, (w, h, color) in PLATFORMS.items():
        dest = OUT / plat
        dest.mkdir(parents=True, exist_ok=True)
        for scene in SCENES:
            shot = render(w, h, color, scene)
            path = dest / scene["file"]
            shot.save(path, "PNG", optimize=True)
            print(f"wrote {path.relative_to(ROOT)} ({w}x{h})")
    print("Done:", OUT)


if __name__ == "__main__":
    main()
