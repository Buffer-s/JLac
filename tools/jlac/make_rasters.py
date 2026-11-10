#!/usr/bin/env python3
"""Builds the raster brand assets (PNG / ICO / ICNS) from the vector sources.

    python3 tools/jlac/make_rasters.py

Outputs (all committed, so normal builds don't need ImageMagick):
  program_info/jlac_256.png   Linux 256px icon
  program_info/JL.ico        Windows icon (multi-size)
  program_info/JL.icns       macOS icon (multi-size)
"""
import pathlib
import struct
import sys
import tempfile

sys.path.insert(0, str(pathlib.Path(__file__).parent))
from svg_render import render  # noqa: E402

ROOT = pathlib.Path(__file__).resolve().parents[2]
INFO = ROOT / "program_info"
ICO_SIZES = [16, 24, 32, 48, 64, 128, 256]


def png_bytes(path: pathlib.Path) -> bytes:
    return path.read_bytes()


def make_ico(pngs: list[pathlib.Path], out: pathlib.Path) -> None:
    """Minimal multi-image ICO writer (PNG-compressed entries, Vista+)."""
    header = struct.pack("<HHH", 0, 1, len(pngs))
    entries, blobs, offset = b"", b"", 6 + 16 * len(pngs)
    for png in pngs:
        data = png_bytes(png)
        size = int(png.stem.rsplit("_", 1)[-1])
        entries += struct.pack("<BBBBHHII", size % 256, size % 256, 0, 0, 1, 32, len(data), offset)
        blobs += data
        offset += len(data)
    out.write_bytes(header + entries + blobs)


def make_icns(pngs: list[pathlib.Path], out: pathlib.Path) -> None:
    """Minimal ICNS writer using the modern PNG-backed types."""
    # icns type -> pixel size
    types = {16: b"icp4", 32: b"icp5", 64: b"icp6", 128: b"ic07", 256: b"ic08", 512: b"ic09", 1024: b"ic10"}
    body = b""
    for png in pngs:
        size = int(png.stem.rsplit("_", 1)[-1])
        if size not in types:
            continue
        data = png_bytes(png)
        body += types[size] + struct.pack(">I", len(data) + 8) + data
    out.write_bytes(b"icns" + struct.pack(">I", len(body) + 8) + body)


def main() -> int:
    with tempfile.TemporaryDirectory() as tmp:
        tmpdir = pathlib.Path(tmp)
        pngs = []
        for size in sorted(set(ICO_SIZES + [512, 1024])):
            png = tmpdir / f"jlac_{size}.png"
            render(INFO / "jlac.svg", png, size)
            pngs.append(png)

        # Linux 256px icon used by the AppImage / desktop integration
        (INFO / "jlac_256.png").write_bytes(png_bytes(tmpdir / "jlac_256.png"))

        make_ico([p for p in pngs if int(p.stem.rsplit("_", 1)[-1]) in ICO_SIZES], INFO / "JL.ico")

        icns_src = []
        for size in (16, 32, 64, 128, 256, 512, 1024):
            png = tmpdir / f"bigsur_{size}.png"
            render(INFO / "jlac.bigsur.svg", png, size)
            icns_src.append(png)
        make_icns(icns_src, INFO / "JL.icns")

    for name in ("jlac_256.png", "JL.ico", "JL.icns"):
        print("wrote", (INFO / name).relative_to(ROOT))
    return 0


if __name__ == "__main__":
    sys.exit(main())
