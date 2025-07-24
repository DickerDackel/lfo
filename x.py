#!/usr/bin/env python3

from time import sleep

from lfo import LFO

l = LFO()
while True:
    print(f'{l.t=:12.8}  {l.normalized=:12.8}  {l.sine=:12.8}  {l.triangle=:12.8}  {l.sawtooth=:12.8}  {l.square=:12.8} {l.pw=}')
    sleep(0.25)
