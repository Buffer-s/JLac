"""Extracts the 'JL' wordmark outline from DejaVu Sans Bold into an SVG path.

The icon sources in program_info/ are generated from this outline, so the
wordmark stays a real vector at every size. Run once (requires fontTools):

    PYTHONPATH=/path/to/fonttools python3 tools/jlac/glyphs.py > tools/jlac/jl_path.txt
"""
import sys

from fontTools.pens.boundsPen import BoundsPen
from fontTools.pens.svgPathPen import SVGPathPen
from fontTools.pens.transformPen import TransformPen
from fontTools.ttLib import TTFont

FONT = "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"

font = TTFont(FONT)
gs = font.getGlyphSet()
upm = font["head"].unitsPerEm
cmap = font.getBestCmap()

names = [cmap[ord(c)] for c in "JL"]
advances = []
for name in names:
    bounds = BoundsPen(gs)
    gs[name].draw(bounds)
    advances.append(gs[name].width)

x = 0
parts = []
for name, advance in zip(names, advances):
    pen = SVGPathPen(gs, ntos=lambda v: f"{v:.2f}".rstrip("0").rstrip("."))
    gs[name].draw(TransformPen(pen, (1, 0, 0, -1, x, 0)))
    parts.append(pen.getCommands())
    x += advance

# Tight ink bounds of the whole wordmark, in font units (y grows up, baseline at 0).
boxes = []
x = 0
for name, advance in zip(names, advances):
    pen = BoundsPen(gs)
    gs[name].draw(TransformPen(pen, (1, 0, 0, 1, x, 0)))
    boxes.append(pen.bounds)
    x += advance

xmin = min(b[0] for b in boxes)
xmax = max(b[2] for b in boxes)
ymin = min(b[1] for b in boxes)
ymax = max(b[3] for b in boxes)

print(f"{upm} {xmin:.1f} {ymin:.1f} {xmax:.1f} {ymax:.1f}", file=sys.stderr)
print("".join(parts))
