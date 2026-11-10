#!/usr/bin/env python3
"""Generates the JL icon set from the wordmark outline in jl_path.txt.

The wordmark is real vector data (outlines of "JL" from DejaVu Sans Bold), so the
SVG sources stay crisp at any size while packagers still get raster assets.

    PYTHONPATH=/path/to/fonttools python3 tools/jlac/glyphs.py > tools/jlac/jl_path.txt
    python3 tools/jlac/make_branding.py
"""
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
PATH_D = (pathlib.Path(__file__).parent / "jl_path.txt").read_text().strip()

# Ink bounds of the wordmark once the font outline has been flipped into SVG space
# (the extracted path is already y-down, so "top" is the largest value).
# Bounds of the extracted outline, in its own (y-down) coordinate space.
XMIN, XMAX = -115.0, 2011.0
YMIN, YMAX = -1493.0, 410.0
INK_W, INK_H = XMAX - XMIN, YMAX - YMIN

BLACK = "#0B0B0C"
WHITE = "#FFFFFF"


def wordmark(canvas: float, ink_ratio: float) -> str:
    """A <path> drawing the wordmark centred in a `canvas` square."""
    scale = canvas * ink_ratio / INK_H
    tx = canvas / 2 - scale * (XMIN + XMAX) / 2
    ty = canvas / 2 - scale * (YMIN + YMAX) / 2
    return (
        f'<path transform="translate({tx:.2f} {ty:.2f}) scale({scale:.6f})" '
        f'd="{PATH_D}" fill="{WHITE}"/>'
    )


def tile_svg(canvas: int, tile: float, radius: float, ink_ratio: float) -> str:
    """Black rounded tile with the white wordmark. `tile` is the tile edge length."""
    inset = (canvas - tile) / 2
    return (
        '<?xml version="1.0" encoding="UTF-8"?>\n'
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{canvas}" height="{canvas}" '
        f'viewBox="0 0 {canvas} {canvas}">\n'
        "  <title>JL</title>\n"
        f'  <rect x="{inset:.2f}" y="{inset:.2f}" width="{tile:.2f}" height="{tile:.2f}" '
        f'rx="{radius:.2f}" ry="{radius:.2f}" fill="{BLACK}"/>\n'
        f"  {wordmark(tile, ink_ratio)}\n"
        "</svg>\n"
    )


def write(path: pathlib.Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text)
    print("wrote", path.relative_to(ROOT))


def main() -> int:
    info = ROOT / "program_info"
    res = ROOT / "launcher" / "resources"

    # Full-bleed tile for Linux/Windows; macOS gets the usual inset rounded square.
    write(info / "jlac.svg", tile_svg(512, 512, 116, 0.34))
    write(info / "jlac.bigsur.svg", tile_svg(1024, 824, 185, 0.34))

    # Same mark, sized for in-app use and for the instance icon list.
    write(res / "multimc/scalable/launcher.svg", tile_svg(48, 48, 11, 0.36))
    write(res / "multimc/scalable/instances/jlac.svg", tile_svg(48, 48, 11, 0.36))

    # macOS asset catalog sources.
    write(info / "PrismLauncher.icon/Assets/block.svg", tile_svg(512, 512, 116, 0.34))
    write(info / "PrismLauncher.icon/Assets/rainbow.svg", tile_svg(512, 512, 116, 0.34))
    return 0


if __name__ == "__main__":
    sys.exit(main())
