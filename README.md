# LFO - A Low Frequency Oscillator

_One of my many interests is playing the modular synth.  That instrument is
not imaginable without the help of LFOs.  They are used to control every
controllable knob or slider, they control the speed of oscillators, the fading
in and out of filters, they can control each other, the possibilities are
literally endless._


## Synopsis

```python
lfo = LFO(period, *, ...)
```

```python

from lfo import LFO
from time import sleep

orbit = LFO(10)
while orbit.cycle < 3:
    print(f'{orbit.sine=} {orbit.cosine=}  {orbit.triangle=}  {orbit.sawtooth=}  {orbit.square=}')
    print(f'{orbit.inv_sine=} {orbit.inv_cosine=}  {orbit.inv_triangle=}  {orbit.inv_sawtooth=}  {orbit.inv_square=}')
    sleep(0.1)
```


## What is an LFO?

So what is an LFO?  LFO stands for "Low Frequency Oscillator".  It's a curve
that doesn't stop and that you can pull out values from.  The simplest form is
probably a sine wave.  Regardless of how often you travel along the circle,
you always get consistent and reproducible values out of it.

But LFOs come in many different shapes.  Here are the ones implemented right
now, but I'm open to suggestions to extend this list:

* A sine wave (and a cosine wave that I don't count extra)
* A triangular wave
* A sawtooth wave
* A square wave
* A wave that always outputs 1
* A wave that always outputs 0
* The inverse of all these

The lfo registers the start time of its instantiation.  Its constructor
receives a period, the duration of one single wave until the wave repeats.

When ever you now query a value from the lfo, it gives you the proper
function result of that wave for this specific point in time.  Also, you can
query all of the wave forms from the same lfo.

There's one important difference to the lfo you might know from your DAW or
synth.  Since most programmers will use these to ramp other values by
multiplication, this lfo is not centered around the 0 point of the y axis,
but all waves are positioned so that they return a value between 0 and 1.
There are per-wave parameters to change this.


## Terminology / Parameter Names

### The LFO class

An instance lfo of type LFO offers the following attributes with their default
values:


#### Primary Instancing Attributes, Read/Write

* `lfo.period: float = 1.0`
* `lfo.frequency: float = 1.0` (rw) - Internally an alias for `1 / lfo.period`


#### Wave Outputs, Read-Only

* `lfo.sine: float`, `lfo.inv_sine: float`
* `lfo.cosine: float`, `lfo.inv_cosine: float`
* `lfo.triangle: float`, `lfo.inv_triangle: float`
* `lfo.sawtooth: float`, `lfo.inv_sawtooth: float`
* `lfo.square: float`, `lfo.inv_square: float`
* `lfo.one: float`, `lfo.inv_one: float`
* `lfo.zero: float`, `lfo.inv_zero: float`


#### Waveform Control Parameters

* `lfo.sine_attenuverter: float = 1.0`
* `lfo.sine_offset: float = 0.0`
* `lfo.cosine_attenuverter: float = 1.0`
* `lfo.cosine_offset: float = 0.0`
* `lfo.triangle_attenuverter: float = 1.0`
* `lfo.triangle_offset: float = 0.0`
* `lfo.sawtooth_attenuverter: float = 1.0`
* `lfo.sawtooth_offset: float = 0.0`
* `lfo.square_attenuverter: float = 1.0`
* `lfo.square_offset: float = 0.0`
* `lfo.one_attenuverter: float = 1.0`
* `lfo.one_offset: float = 0.0`
* `lfo.zero_attenuverter: float = 1.0`
* `lfo.zero_offset: float = 0.0`
* `lfo.pw: float = 0.5`
* `lfo.pw_offset: float = 0.0`


#### Read-Only Status Attributes

* `lfo.frozen: bool` - Frozen state of the lfo
* `lfo.t: float` - Time within the current cycle of the lfo
* `lfo.normalized: float` - Like `lfo.t`, but normalized to 0 - 1
* `lfo.cycle: int` - The number of the current cycle of the lfo


### Primary Instancing Attributes, Read/Write

#### `lfo.period`, `lfo.frequency`

`lfo.period` is the primary setting for the lfo.  It's the duration between
wave repeats.

`lfo.frequency` is the inverse of the period.  While the period defines the
duration of one wave cycle, the frequency defines the number of cycles per
second.


### Wave Outputs, Read-Only

If you have ever tried making sounds on a computer, you will be very familiar
with the available wave types.

All wave forms come with an inverted version named `inv_<waveform>`.


#### Sine, Cosine (And important configuration parameters!)

    `lfo.sine`, `lfo.cosine`, `lfo.inv_sine`, `lfo.inv_cosine`

Your off-the-mill sine and cosine waves.

**Note**: In the first iteration of this library, I thought it was a good idea
to position the sine and cosine waves in the range or 0 - 1.  This idea does
not work well in reality, so they now deliver the values any programmer will
expect.

See `<waveform>_attenuverter` and `<waveform>_offset` below on how to change
this.

#### Triangle

    `lfo.triangle`, `lfo.inv_triangle`

A triangle wave ramps up from 0 to 1 over half of the period.  Then it ramps
down back to zero for the second half, creating a triangular shape.

#### Sawtooth

    `lfo.sawtooth`, `lfo.inv_sawtooth`

A sawtooth wave starts at 1 and ramps down to 0 over the full length of the
period.


#### Square

    `lfo.square`, `lfo.inv_square`

The square wave holds 0 over a given time that defaults to half the period,
then it switches to 1.

See `lfo.pw` and `lfo.pw_offset` below for some configuration options.


#### One, Zero

    `lfo.one`, `lfo.inv_one`, `lfo.zero`, `lfo.inv_zero`

While it may sound useless to have an object that puts out a constant value,
one and zero can be very useful if you want to deactivate an changing
behaviour in an object without adding a special if-clause or changing its
interface.


### Waveform Control Parameters

These settings all control the different wave forms.  They are configurable
both at class instantiation, as well as in runtime.

The consequence of that is that they can be modulated by another lfo...

#### `<waveform>_attenuverter`, `<waveform>.offset`

All waveforms offer these two modifiers.  They can passed to the `LFO()` init
when instantiating, and also during runtime

The weird term _attenuverter_ also comes from the world of modular synths and
is a combination of _attenuator_ - a scale factor - and _inverter_ - because a
negative scale will invert the wave.

A good use case for these settings are the sine and cosine waves, which return
values between -1 and 1.  If you want to use them for scaling something
instead, you set their attenuverter to 0.5.  Now they return values from -0.5
to 0.5.  Then you offset them by 0.5 and you have your scaling factor.

So the attenuverter and offset alwasy return

    `wave * attenuverter + offset`

```
lfo = LFO(period, sine_attenuverter=1, sine_offset=0, ...)
```

This first scales the sine wave back to the range of -1 to 1, and then removes
the position shift from the defaults.

Even `lfo.one` and `lfo.zero` are impacted by these.

#### `lfo.pw` and `lfo.pw_offset`

**NOTE**: Mighth be renamed to `square_pw` and `square_pw_offset`.

*pw* stands for pulse width.  In a synth, it's the duration a signal is up or
down.  By default, the square wave is up for half of the period, then switches
to down.  It's pulse width (up) is 0.5.  Setting `lfo.pw = 1/3` instead will
make the square wave generate a `1` for the first 3rd of the period, and a `0`
for the remaining time.

**Note**: The value of the pulse width parameters is normalized to 0-1.  That
way, you won't have to modify it every time you change the period of the LFO.

The `lfo.pw_offset` now shifts the start position of the *pulse*.  By default,
it starts at the beginning of the period.  But if you would want a narrow
pulse in the middle of the wave, you can shift the start.

```python
lfo = LFO(10, pw=2, pw_offset=4)
```

Will result in a square wave that return `0` until the start of the 4th
second.  Then it will switch to `1` from second 4 to second 6.  Finally, it
will be back to `0` from second 6 until the end of the period at second 10.

So with `lfo.pw` you control the width of the "on-phase", and with
`lfo.pw_offset` you control its position.


### Read-Only Status Attributes

#### `lfo.t`, `lfo.normalized`

`lfo.t` will give you the current time within the current cycle of the curve.
This will be a value between 0 and `lfo.period`.

`lfo.normalized` will give you the same, but scaled into the range of 0 to 1.

Both attributes will reset after each period.


#### `lfo.cycles`

The number of the loop that the lfo is currently in.  Increments after each
period.

### Methods

_An LFO is mostly a fire and forget object.  But it still offers a small
handful of methods_

#### `lfo.reset() -> None`

Resets the start time of the lfo to the current time.  With short `periods`,
this will most likely not be relevant, but an lfo can also run for a very long
time, e.g. to ramp up enemy spawns over a level, and you don't want to have
it deep in its cycle when the next level begins.

#### `lfo.freeze() -> None`, `lfo.unfreeze() -> None`, `lfo.is_frozen() -> bool`

Pauses the lfo.  The current value is held until the pause is terminated.
Note that the lfo's start time shifts so, that the wave it outputs is
continuous.  It will **not** jump once it's reactivated.

If you prefer a function interface over the status attribute `lfo.frozen`
above, use `lfo.is_frozen()`, which returns a bool.

### Python Magic Methods

LFO instances properly convert to `bool`, `int` and `float` and can thus be
directly compared to all of them.

Note that as of now, the `sine` curve is used.  This might be configurable
later...  FIXME

So e.g. `bool(lfo)` is the same as `bool(lfo.sine)`.

LFO instances also provide `__call__`, so if you prefer the function
interface, use `lfo()` to fetch the sine value from the lfo.

In addition, lfo is both an iterator and iterable.

```python
from lfo import LFO
from time import sleep

l = LFO(10)
next(l)
>>> 0.2457116119475273

for i, v in zip(range(10), l):
    print(i, v)
    sleep(0.25)

>>> 0 -0.9886300710248443
>>> 1 -0.9999805639536082
>>> 2 -0.9866305171025227
>>> 3 -0.9488620508175084
>>> 4 -0.8876400367987355
>>> 5 -0.804481428399949
>>> 6 -0.7013677240075332
>>> 7 -0.5811011443144513
>>> 8 -0.44612926127688374
>>> 9 -0.3002298909230939
```


## Example

_NOTE 1: to preserve the zero-dependencies of lfo, examples/demos will be
published in a dedicated package lfo_demos, which is currently in the making
but not published yet.

_NOTE 2: This example requires `pygame-ce`, but the code should be straight
forward enough to be directly translated to other frameworks like `pyglet`.

**NOTE 3: Please don't use the badly maintained `pygame` project anymore.**

```python
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
SATELITE_RADIUS = 32

orbit = LFO(10, sine_offset=0.0, cosine_offset=0.0)
satelite = LFO(5, sine_offset=0.0, cosine_offset=0.0)
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

    x += satelite.cosine * SATELITE_RADIUS
    y += satelite.sine * SATELITE_RADIUS
    renderer.draw_color = ('cyan', 'orange')[int(color.square)]
    renderer.draw_rect(rect.move_to(center=(round(x), round(y))))

    satelite.period = speedo.sine

    x = orbit.sine * 2 * RADIUS + SCREEN.centerx + satelite.sine * SATELITE_RADIUS
    y = orbit.cosine * 2 * RADIUS + SCREEN.centery + satelite.cosine * SATELITE_RADIUS
    renderer.draw_color = ('green', 'red')[int(color.square)]
    renderer.draw_rect(rect.scale_by(2).move_to(center=(round(x), round(y))))


    renderer.present()

    window.title = f'{TITLE} - time={pygame.time.get_ticks()/1000:.2f}  fps={clock.get_fps():.2f}'
```

## Installation

Note that this module is in very early draft, and while it is already useful
and functional, things might change...


### Installation via pip

lfo is available on pypi and can be installed by

```console
pip install lfo
```

(You are using venvs, right?  RIGHT?!?)

lfo comes with no additional requirements, but it is very good to create input
for easing functions from `rpeasings`.  lfo was primarily created as a tool
for my pygame projects, but it's a generalized control tool that can be used
in many different scenarios and environments.


### Pre-built wheels for Windows, Mac and Linux...

...are available on the github releases page at

https://github.com/dickerdackel/lfo/releases


### Install from source

Again, use a venv!

lfo is packaged following the python packaging authority at https://www.pypa.io/

You can simply clone the github repo from https://github.com/dickerdackel/lfo

Then change into this directory and install the package - INTO YOUR VENV! -
with `pip install .`

```console
# clone repo
dickerdackel@minime:~$ git clone https://github.com/dickerdackel/lfo
...

# create venv
dickerdackel@minime:~/lfo$ python3 -m venv --prompt lfo .venv

# activate venv
dickerdackel@minime:~/lfo$ . .venv/bin/activate

# install lfo
(lfo) dickerdackel@minime:~/lfo$ pip install .

(lfo) dickerdackel@minime:~/lfo$
```

## Support / Contributing

Issues can be opened on [Github](https://github.com/dickerdackel/lfo/issues)

## Credits / Acknowledgements

* Thanks to all the modular synth vendors that showed me the versatility of
  LFOs

## License

This software is provided under the MIT license.

See LICENSE file for details.
