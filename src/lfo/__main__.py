#!/usr/bin/env python3

from collections import defaultdict

import pygame

from pygame import Vector2 as vec2

from lfo import LFO

TITLE = 'pygame minimal template'
SCREEN = pygame.Rect(0, 0, 1280, 720)
FPS = 60
DT_MAX = 3 / FPS

GAUGES = ('sine', 'inv_sine',
          'cosine', 'inv_cosine',
          'triangle', 'inv_triangle',
          'sawtooth', 'inv_sawtooth',
          'square', 'inv_square')

gauge_len = len(GAUGES)
DURATION = 3
MARGIN = 10
COLUMNS = 4
ROWS = gauge_len // COLUMNS + 1 if gauge_len % COLUMNS else 0
CANVAS_SIZE = (SCREEN.width / COLUMNS, SCREEN.height / ROWS)

MARKER = pygame.Surface((10, 10), pygame.SRCALPHA)
pygame.draw.circle(MARKER, 'green', (5, 5), 5)

pygame.init()
clock = pygame.time.Clock()
pygame.display.set_caption(TITLE)
screen = pygame.display.set_mode(SCREEN.size)

class ZeSprite(pygame.sprite.Sprite):
    def __init__(self, pos, image, *groups):
        super().__init__(*groups)
        self.pos = vec2(pos)
        self.image = image.copy()
        self.rect = image.get_rect(center=pos)

    def update(self, dt):
        self.rect.center = self.pos

class FadeSprite(ZeSprite):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.fade = LFO(DURATION, sawtooth_attenuverter = 255)

    def update(self, dt):
        if self.fade.cycle != 1:
            self.kill()

        self.image.set_alpha(self.fade.sawtooth)

def main():
    canvases = {}
    rect = pygame.Rect(0, 0, *CANVAS_SIZE)
    for i, gauge in enumerate(GAUGES):
        y = i // COLUMNS * CANVAS_SIZE[1]
        x = i % COLUMNS * CANVAS_SIZE[0]

        canvases[gauge] = pygame.Rect(x, y, *CANVAS_SIZE).inflate((-MARGIN, -MARGIN))

    font = pygame.font.Font(None, 48)
    group = pygame.sprite.Group()
    lfo = LFO(DURATION, sine_attenuverter=0.5, sine_offset=0.0, cosine_attenuverter=0.5, cosine_offset=0.0)
    print(lfo.sine_attenuverter, lfo.sine_offset, lfo.cosine_attenuverter, lfo.cosine_offset)

    running = True
    pw = LFO(2 * DURATION, sine_attenuverter=0.5, sine_offset=0.5)
    prev_val = defaultdict(int)
    while running:
        dt = min(clock.tick() / 1000.0, DT_MAX)

        for e in pygame.event.get():
            if e.type == pygame.QUIT:
                running = False
            elif e.type == pygame.KEYDOWN:
                if e.key == pygame.K_ESCAPE:
                    running = False
        screen.fill('black')

        pygame.draw.rect(screen, 'red', SCREEN, width=1)

        lfo.pw = pw.sine
        for gauge in GAUGES:
            rect = canvases[gauge]
            label = font.render(gauge, True, 'blue')
            screen.blit(label, label.get_rect(center=rect.center))

        for gauge, canvas in canvases.items():
            if gauge in ('sine', 'inv_sine', 'cosine', 'inv_cosine'):
                origin = vec2(canvas.midleft)
            else:
                origin = vec2(canvas.bottomleft)

            pygame.draw.rect(screen, 'darkslategrey', canvas, width=0)
            pygame.draw.line(screen, 'yellow', origin, (canvas.right, origin[1]), width=1)

            t = lfo.normalized * canvas.width
            pygame.draw.line(screen, 'yellow', origin + (t, -5), origin + (t, +5), 1)

            # if gauge != focus: continue

            val = getattr(lfo, gauge)
            y = val * canvas.height
            pos = origin + (t, -y)
            group.add(FadeSprite(pos, MARKER))

            print(LFO.pw)
            if gauge in ['square', 'inv_square'] and prev_val[gauge] != val:
                for i in range(10):
                    y = (i / 10) * canvas.height
                    pos = origin + (t, -y)
                    group.add(FadeSprite(pos, MARKER))

            prev_val[gauge] = val



        group.update(dt)
        group.draw(screen)

        pygame.display.flip()
        pygame.display.set_caption(f'{TITLE} - time={pygame.time.get_ticks()/1000:.2f}  fps={clock.get_fps():.2f}')


if __name__ == "__main__":
    main()
