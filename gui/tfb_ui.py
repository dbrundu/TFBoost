"""Optional uniform UI scaling for the fixed-pixel Tkinter layout.

The GUI is laid out with hard-coded pixel geometry, ``place()`` coordinates,
canvas sizes and point-size fonts tuned for a ~96 dpi display. On HiDPI /
fractional-scaling desktops (especially XWayland, which reports a fake 96 dpi to
its clients) the windows come out physically tiny and cramped.

Setting the ``TFB_UI_SCALE`` environment variable multiplies window geometry,
widget positions, canvas sizes and fonts by a single factor, so the whole UI
grows uniformly and keeps its proportions. ``start.sh`` sets it automatically
from the monitor scale; override it by exporting ``TFB_UI_SCALE=<factor>``
(e.g. 1.25, 1.5, 2.0) before launching.

Importing this module installs the geometry/place/canvas patches; call
``apply_font_scaling(root)`` once on each Tk root to scale the fonts too.
"""
import os
import re
import tkinter as tk

try:
    SCALE = float(os.environ.get('TFB_UI_SCALE', '') or 1.0)
except ValueError:
    SCALE = 1.0

# points -> pixels at the display the layout was designed for (96 dpi)
_DESIGN_TK_SCALING = 96.0 / 72.0


def _scaled(value):
    try:
        return int(round(float(value) * SCALE))
    except (TypeError, ValueError):
        return value


def _install_patches():
    """Scale the *pixel* quantities: place() x/y/width/height, toplevel
    geometry, and Canvas width/height. Character-based sizes (Entry/Text/Button
    width) are left alone — they follow the font, which we scale separately."""
    if SCALE == 1.0:
        return

    _place = tk.Place.place_configure

    def place_configure(self, cnf={}, **kw):
        merged = dict(cnf)
        merged.update(kw)
        for key in ('x', 'y', 'width', 'height'):
            if merged.get(key) not in (None, ''):
                merged[key] = _scaled(merged[key])
        return _place(self, merged)

    tk.Place.place_configure = place_configure
    tk.Place.place = place_configure

    _geometry = tk.Wm.wm_geometry

    def wm_geometry(self, newGeometry=None):
        if newGeometry:
            m = re.match(r'^(\d+)x(\d+)(.*)$', newGeometry)
            if m:
                newGeometry = "%dx%d%s" % (_scaled(m.group(1)),
                                           _scaled(m.group(2)), m.group(3))
        return _geometry(self, newGeometry)

    tk.Wm.wm_geometry = wm_geometry
    tk.Wm.geometry = wm_geometry

    _canvas_init = tk.Canvas.__init__

    def canvas_init(self, master=None, cnf={}, **kw):
        for key in ('width', 'height'):
            if key in kw:
                kw[key] = _scaled(kw[key])
        _canvas_init(self, master, cnf, **kw)

    tk.Canvas.__init__ = canvas_init


def apply_font_scaling(root):
    """Scale point-size fonts to match the geometry scaling. Call once per root."""
    if SCALE != 1.0:
        try:
            root.tk.call('tk', 'scaling', _DESIGN_TK_SCALING * SCALE)
        except tk.TclError:
            pass


_install_patches()
