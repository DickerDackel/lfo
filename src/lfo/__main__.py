#!/bin/env python3

from collections import defaultdict
from itertools import chain, cycle
from types import SimpleNamespace

import pygame
import pygame._sdl2 as sdl2

from lfo import LFO
from pygame import Vector2 as vec2

pygame.font.init()

TITLE = 'pygame minimal template'
SCREEN = pygame.Rect(0, 0, 1280, 720)
FPS = 60
DT_MAX = 3 / FPS

GAUGES = ('sine', 'inv_sine',
          'cosine', 'inv_cosine',
          'triangle', 'inv_triangle',
          'sawtooth', 'inv_sawtooth',
          'square', 'inv_square')

COLORS = SimpleNamespace(background='darkslategrey',
                         canvas='yellow',
                         pen='green')

gauge_len = len(GAUGES)
DURATION = 1
MARGIN = 10
COLUMNS = 4
ROWS = gauge_len // COLUMNS + 1 if gauge_len % COLUMNS else 0
CANVAS_SIZE = (SCREEN.width / COLUMNS, SCREEN.height / ROWS)
FONT = pygame.font.Font(None, 48)


class ZeGroup(pygame.sprite.Group):
    def draw(self, *args, **kwargs):
        for sprite in self.sprites():
            sprite.draw()


class ZeSprite(pygame.sprite.Sprite):
    def __init__(self, pos, texture, *groups):
        super().__init__(*groups)
        self.pos = vec2(pos)
        self.image = texture
        self.rect = texture.get_rect(center=pos)

    def update(self, dt):
        self.rect.center = self.pos

    def draw(self):
        self.image.draw(dstrect=self.rect)


class FadeSprite(ZeSprite):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.fade = LFO(DURATION / 2, sawtooth_attenuverter=255)
        self.scale = LFO(DURATION, sawtooth_attenuverter=1)

    def update(self, dt):
        if self.fade.cycle > 1 or self.scale.cycle > 1:
            self.kill()

    def draw(self):
        bkp_alpha = self.image.alpha
        self.image.alpha = self.fade.sawtooth
        # self.image.draw(dstrect=self.rect.scale_by(self.scale.sawtooth))
        self.image.draw(dstrect=self.rect)
        self.image.alpha = bkp_alpha


def mk_pen(renderer):
    image = pygame.Surface((10, 10), pygame.SRCALPHA)
    pygame.draw.circle(image, COLORS.pen, (5, 5), 5)
    texture = sdl2.Texture.from_surface(renderer, image)
    texture.blend_mode = pygame.BLENDMODE_BLEND

    return texture


def mk_canvases():
    canvases = {}
    for i, gauge in enumerate(GAUGES):
        y = i // COLUMNS * CANVAS_SIZE[1]
        x = i % COLUMNS * CANVAS_SIZE[0]

        canvases[gauge] = pygame.Rect(x, y, *CANVAS_SIZE).inflate((-MARGIN, -MARGIN))

    return canvases


def mk_labels(renderer, canvases):
    res = ZeGroup()
    for gauge in GAUGES:
        canvas = canvases[gauge]
        image = FONT.render(gauge, True, 'cyan')
        texture = sdl2.Texture.from_surface(renderer, image)
        res.add(ZeSprite(canvas.center, texture))

    return res


def update_canvases(canvases, lfo, renderer, pen, prev_y):
    res = []
    bkp_color = renderer.draw_color
    for gauge, canvas in canvases.items():
        # draw 0 line in center for sin/cos, bottom for others
        if gauge in ('sine', 'inv_sine', 'cosine', 'inv_cosine'):
            origin = vec2(canvas.midleft)
        else:
            origin = vec2(canvas.bottomleft)

        renderer.draw_color = 'darkslategrey'
        renderer.draw_rect(canvas)

        renderer.draw_color = COLORS.canvas
        renderer.draw_rect(canvas)
        renderer.draw_line(origin, (canvas.right, origin[1]))

        t = lfo.normalized * canvas.width
        renderer.draw_line(origin + (t, -5), origin + (t, +5))

        y = getattr(lfo, gauge) * canvas.height
        pos = origin + (t, -y)
        res.append(FadeSprite(pos, pen))

        if gauge in ('square', 'inv_square') and prev_y[gauge] != y:
            prev_y[gauge] = y

            for i in range(25):
                y = (i / 25) * canvas.height
                pos = origin + (t, -y)
                res.append(FadeSprite(pos, pen))

    renderer.draw_color = bkp_color

    return res


def main():
    clock = pygame.time.Clock()
    # window = pygame.Window(title=TITLE, size=SCREEN.size)
    window = pygame.Window(title=TITLE, fullscreen_desktop=True)
    renderer = sdl2.Renderer(window)
    renderer.logical_size = SCREEN.size

    pen = mk_pen(renderer)
    canvases = mk_canvases()

    pen_group = ZeGroup()
    label_group = mk_labels(renderer, canvases)

    lfo = LFO(DURATION, sine_attenuverter=0.5, sine_offset=0.0, cosine_attenuverter=0.5, cosine_offset=0.0)
    pw = LFO(DURATION, sine_attenuverter=0.4)
    other_pw = cycle(i / 10 for i in chain(range(10), range(8, 2, -1)))

    prev_y = defaultdict(int)  # for (inv_)square vertical line

    prev_cycle = 0
    running = True
    while running:
        dt = min(clock.tick(0) / 1000.0, DT_MAX)

        for e in pygame.event.get():
            if e.type == pygame.QUIT:
                running = False
            elif e.type == pygame.KEYDOWN:
                if e.key == pygame.K_ESCAPE:
                    running = False

        renderer.draw_color = 'darkslategrey'
        renderer.clear()

        label_group.update(dt)
        pen_group.update(dt)

        # lfo.pw = pw.sine
        if lfo.cycle != prev_cycle:
            print(lfo.pw)
            lfo.pw = next(other_pw)
            prev_cycle = lfo.cycle

        label_group.draw(renderer)
        sprites = update_canvases(canvases, lfo, renderer, pen, prev_y)
        pen_group.add(sprites)

        pen_group.draw(renderer)
        renderer.present()

        window.title = f'{TITLE} - time={pygame.time.get_ticks()/1000:.2f}  fps={clock.get_fps():.2f}'

if __name__ == "__main__":
    main()
