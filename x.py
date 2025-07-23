#!/usr/bin/env python3

from time import sleep

from lfo import LFO

l = LFO(10)
while True:
    # print(f'{l.sine=}  {l.triangle=}  {l.sawtooth=}  {l.square=}')
    print(f'{l=}  {l.sine=}')
    sleep(0.25)
