"""Rasterises the JL brand SVGs (no librsvg available in the build sandbox).

Kept as a maintenance tool: the outputs are committed, so a normal build does not
need ImageMagick. Renders program_info/jlac*.svg at any size.
"""
import pathlib
import re
import subprocess
import sys


def _parse(svg: pathlib.Path):
    text = svg.read_text()
    rect = re.search(r'<rect x="([\d.]+)" y="([\d.]+)" width="([\d.]+)" height="([\d.]+)" rx="([\d.]+)"', text)
    path = re.search(r'transform="translate\(([-\d.]+) ([-\d.]+)\) scale\(([-\d.]+)\)" d="([^"]+)"', text)
    values = [float(v) for v in rect.groups()] + [float(v) for v in path.groups()[:3]]
    return values, path.group(4)


def render(svg: pathlib.Path, png: pathlib.Path, size: int) -> None:
    (rx, ry, rw, rh, rr, tx, ty, scale), d = _parse(svg)
    # The source artboard is 512px; scale everything to the requested size.
    k = size / 512
    toks = re.findall(r'([A-Za-z])|(-?\d*\.?\d+)', d)
    seq = [c if c else float(n) for c, n in toks]
    fmt = lambda x, y: f"{(tx + scale * x) * k:.2f} {(ty + scale * y) * k:.2f}"
    out, j, cmd, cx, cy, ax, ay = [], 0, None, 0.0, 0.0, 0.0, 0.0
    while j < len(seq):
        token = seq[j]
        if isinstance(token, str):
            cmd = token
            j += 1
            if cmd == "Z":
                out.append("Z")
                cx, cy = ax, ay
            continue
        if cmd == "M":
            cx, cy = seq[j], seq[j + 1]
            ax, ay = cx, cy
            out.append("M " + fmt(cx, cy))
            j += 2
            cmd = "L"
        elif cmd == "L":
            cx, cy = seq[j], seq[j + 1]
            out.append("L " + fmt(cx, cy))
            j += 2
        elif cmd == "H":
            cx = seq[j]
            out.append("L " + fmt(cx, cy))
            j += 1
        elif cmd == "V":
            cy = seq[j]
            out.append("L " + fmt(cx, cy))
            j += 1
        elif cmd == "Q":
            qx, qy, x, y = seq[j : j + 4]
            out.append("Q " + fmt(qx, qy) + " " + fmt(x, y))
            cx, cy = x, y
            j += 4
        else:
            raise SystemExit(f"unhandled path command {cmd}")

    mvg = (
        f"fill '#0B0B0C' roundrectangle {rx * k:.2f},{ry * k:.2f},{(rx + rw) * k - 1:.2f},{(ry + rh) * k - 1:.2f},"
        f"{rr * k:.2f},{rr * k:.2f}\nstroke none\nfill white\npath '" + " ".join(out) + "'\n"
    )
    draw = png.with_suffix(".mvg")
    draw.write_text(mvg)
    subprocess.run(
        ["convert", "-background", "none", "-size", f"{size}x{size}", "xc:none", "-draw", f"@{draw}", str(png)],
        check=True,
    )
    draw.unlink()


if __name__ == "__main__":
    render(pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2]), int(sys.argv[3]))
