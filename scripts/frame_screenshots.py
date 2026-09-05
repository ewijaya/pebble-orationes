#!/usr/bin/env python3
"""Copy Emery screenshots and place them in a consistent PT2-style frame."""

from __future__ import annotations

import argparse
import shutil
import sys
from pathlib import Path

try:
    from PIL import Image, ImageDraw
except ImportError:
    print(
        "Pillow is required. Install it with: python3 -m pip install Pillow",
        file=sys.stderr,
    )
    raise SystemExit(1)


CANVAS_SIZE = (600, 800)
SCREEN_BOX = (100, 172, 500, 628)
OUTPUT_ALIASES = {
    "launcher-icon": "launcher",
    "holy-rosary-menu": "rosary-menu",
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--input-dir",
        type=Path,
        default=Path("build/emulator-screenshots"),
        help="directory containing raw PNG screenshots",
    )
    parser.add_argument(
        "--raw-dir",
        type=Path,
        default=Path("docs/images/raw"),
        help="directory for repository copies of raw screenshots",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path("build/framed-screenshots"),
        help="directory for optional generated frames (ignored by Git)",
    )
    return parser.parse_args()


def draw_watch_frame() -> Image.Image:
    canvas = Image.new("RGB", CANVAS_SIZE, "#F3F4F6")
    draw = ImageDraw.Draw(canvas)

    # Flat strap segments sit behind the case.
    draw.rounded_rectangle((206, -24, 394, 132), radius=28, fill="#292B2E")
    draw.rounded_rectangle((206, 668, 394, 824), radius=28, fill="#292B2E")

    # A restrained shadow separates the watch from the neutral background.
    draw.rounded_rectangle((64, 113, 552, 703), radius=48, fill="#D5D7DA")

    # Neutral metal case and high-contrast black bezel.
    draw.rounded_rectangle((56, 105, 544, 695), radius=48, fill="#8B8E93")
    draw.rounded_rectangle((78, 145, 522, 655), radius=32, fill="#090A0B")

    # PT2-inspired side buttons, intentionally simple and unbranded.
    for top in (236, 338, 440):
        draw.rounded_rectangle((542, top, 554, top + 58), radius=5,
                               fill="#303236")
    draw.rounded_rectangle((46, 322, 58, 380), radius=5, fill="#303236")

    return canvas


def fit_screenshot(source: Image.Image) -> Image.Image:
    screen_width = SCREEN_BOX[2] - SCREEN_BOX[0]
    screen_height = SCREEN_BOX[3] - SCREEN_BOX[1]
    screenshot = source.convert("RGB")
    scale = min(
        screen_width / screenshot.width,
        screen_height / screenshot.height,
    )
    fitted_size = (
        round(screenshot.width * scale),
        round(screenshot.height * scale),
    )
    screenshot = screenshot.resize(fitted_size, Image.Resampling.NEAREST)

    fitted = Image.new("RGB", (screen_width, screen_height), "#000000")
    offset = (
        (screen_width - screenshot.width) // 2,
        (screen_height - screenshot.height) // 2,
    )
    fitted.paste(screenshot, offset)
    return fitted


def frame_screenshot(source_path: Path, output_path: Path) -> None:
    with Image.open(source_path) as source:
        fitted = fit_screenshot(source)

    canvas = draw_watch_frame()
    canvas.paste(fitted, SCREEN_BOX[:2])
    canvas.save(output_path, format="PNG", optimize=True)


def main() -> int:
    args = parse_args()
    screenshots = sorted(args.input_dir.glob("*.png"))
    if not screenshots:
        print(f"No PNG screenshots found in {args.input_dir}", file=sys.stderr)
        return 1

    args.raw_dir.mkdir(parents=True, exist_ok=True)
    args.output_dir.mkdir(parents=True, exist_ok=True)

    for source_path in screenshots:
        output_stem = OUTPUT_ALIASES.get(source_path.stem, source_path.stem)
        raw_path = args.raw_dir / f"{output_stem}.png"
        framed_path = args.output_dir / f"{output_stem}.png"
        if source_path.resolve() != raw_path.resolve():
            shutil.copy2(source_path, raw_path)
        frame_screenshot(raw_path, framed_path)
        print(f"{source_path.name} -> {framed_path}")

    print(f"Generated {len(screenshots)} framed screenshot(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
