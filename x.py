#!/bin/env python3

import pygame
import pygame._sdl2 as sdl2

from lfo import LFO

TITLE = 'pygame minimal template'
SCREEN = pygame.Rect(0, 0, 1024, 768)
FPS = 60
DT_MAX = 3 / FPS

clock = pygame.time.Clock()
window = pygame.Window(title=TITLE, size=SCREEN.size)
renderer = sdl2.Renderer(window)

RADIUS = 256
lfo = LFO(2, sine_offset=0.0, cosine_offset=0.0)
rect = pygame.Rect(0, 0, 10, 10)

running = True
while running:
    dt = min(clock.tick(FPS) / 1000.0, DT_MAX)

    for e in pygame.event.get():
        if e.type == pygame.QUIT:
            running = False
        elif e.type == pygame.KEYDOWN:
            if e.key == pygame.K_ESCAPE:
                running = False

    renderer.draw_color = 'darkslategrey'
    renderer.clear()

    renderer.draw_color = 'red'
    renderer.draw_rect(rect.move_to(center=SCREEN.center))

    print(lfo.sine)
    x = lfo.cosine * RADIUS + SCREEN.centerx
    y = lfo.sine * RADIUS + SCREEN.centery
    renderer.draw_color = 'yellow'
    renderer.draw_rect(rect.move_to(center=(x, y)))

    renderer.present()

    window.title = f'{TITLE} - time={pygame.time.get_ticks()/1000:.2f}  fps={clock.get_fps():.2f}'
