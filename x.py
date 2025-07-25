#!/bin/env python3

import pygame
import pygame._sdl2 as sdl2

from lfo import LFO

TITLE = 'pygame minimal template'
SCREEN = pygame.Rect(0, 0, 1024, 768)
FPS = 60
DT_MAX = 3 / FPS

clock = pygame.time.Clock()
window = pygame.Window(title=TITLE, fullscreen_desktop=True)
renderer = sdl2.Renderer(window)
renderer.logical_size = SCREEN.size

RADIUS = 256
SATTELITE_RADIUS = 32

orbit = LFO(10, sine_offset=0.0, cosine_offset=0.0)
sattelite = LFO(5, sine_offset=0.0, cosine_offset=0.0)
speedo = LFO(20, sine_attenuverter=0.3, sine_offset=1.0, cosine_attenuverter=0.3, cosine_offset=1.0)
color = LFO(3)

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

    renderer.draw_color = ('red', 'green')[int(color.square)]

    renderer.draw_rect(rect.move_to(center=SCREEN.center))

    x = orbit.cosine * RADIUS + SCREEN.centerx
    y = orbit.sine * RADIUS + SCREEN.centery
    renderer.draw_color = ('yellow', 'magenta')[int(color.square)]
    renderer.draw_rect(rect.move_to(center=(round(x), round(y))))

    x += sattelite.cosine * SATTELITE_RADIUS
    y += sattelite.sine * SATTELITE_RADIUS
    renderer.draw_color = ('cyan', 'orange')[int(color.square)]
    renderer.draw_rect(rect.move_to(center=(round(x), round(y))))

    sattelite.period = speedo.sine

    x = orbit.sine * 2 * RADIUS + SCREEN.centerx + sattelite.sine * SATTELITE_RADIUS
    y = orbit.cosine * 2 * RADIUS + SCREEN.centery + sattelite.cosine * SATTELITE_RADIUS
    renderer.draw_color = ('green', 'red')[int(color.square)]
    renderer.draw_rect(rect.scale_by(2).move_to(center=(round(x), round(y))))


    renderer.present()

    window.title = f'{TITLE} - time={pygame.time.get_ticks()/1000:.2f}  fps={clock.get_fps():.2f}'
