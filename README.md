# LFO - A Low Frequency Oscillator

_One of my many interests is playing the modular synth.  That instrument is not
imaginable without the help of LFOs.  They are used to control every
controlable knob or slider, they control the speed of oscillators, the fading
in and out of filters, they can control each other, the possibilities are
literally endless._

## What is an LFO?

So what is an LFO?  LFO stands for "Low Frequency Oscillator".  It's a curve
that doesn't stop and that you can pull out values from.  The simplest form is
probably a sine wave.  Regardless of how often you travel along the circle,
you always get consistent and reproducible values out of it.

But LFOs come in many different shapes, 4 of which are implemented here:

* A sine wave (and a cosine wave that I don't count extra)
* A triangular wave
* A sawtooth wave
* A square wave
* The inverse of all these

The lfo registers the start time of its instantiation.  It's constructor
receives a period, the duration of one single wave until the wave repeats.

When ever you now query a value from the lfo, it gives you the proper
functionn result of that wave for this specific point in time.  Also, you can
query all of the wave forms from the same lfo.

Ther'e's one important difference to the lfo you might know from your DAW or
synth.  Since most programmers will use these to ramp other values by
multiplication, this lfo is not centered around the 0 point of the y axis,
but all waves are positioned so, that they return a value between 0 and 1.
There are per-wave parameters to change this.

## Terminology / Parameter Names

#### `lfo.period`

The duration until the wave repeats

#### `lfo.t`, `lfo.normalized`

`lfo.t` will give you the current time within the current cycle of the curve.

`lfo.normalized` will give you the same, but scaled into the range of 0 to 1.

Both attributes will reset after each period.

#### `lfo.cycles`

The number of the loop that the lfo is currently in.  Increments after each
period.

#### `*_attenuverter`, `*_offset`

These scale and reposition the value that comes out of the wave attribute.

The weird term _attenuverter_ also comes from the world of modular synths and
is a combination of _attenuator_ - a scale factor - and _inverter_ - because a
negative scale will invert the wave.

All waves are 
For `sine_attenuverter` and `cosine_attenuverter` are 0.5 by default, to scale
the lfo down to the range -0.5 to 0.5

`sine_offset` and `cosine_offset` then will shift that range into the 0 to 1
segment.  They both default to 0.5.

All `<waveform>_attenuverter` and `<waveform>_offset` parameters can be set as
arguments to `LFO()` and also can be read and assigned in rumtime as
attributes.


## Wave types

If you have ever tried making sounds on a computer, you will be very familiar
with the available wave types.

### Sine, Cosine (And important configuration parameters!)

Your off-the-mill sine and cosine waves.  Note that these are by default
configured so they cycle between 0 and 1, not -1 to 1.

To reset them to the origin, every waveform comes with two parameters:

* `<waveform>_attenuverter`
* `<waveform>_offset`

The weird term _attenuverter_ also comes from the world of modular synths and
is a combination of _attenuator_ - a scale factor - and _inverter_ - because a
negative scale will invert the wave.

For `sine_attenuverter` and `cosine_attenuverter` are 0.5 by default, to scale
the lfo down to the range -0.5 to 0.5

`sine_offset` and `cosine_offset` then will shift that range into the 0 to 1
segment.  They both default to 0.5.

All `<waveform>_attenuverter` and `<waveform>_offset` parameters can be set as
arguments to `LFO()` and also can be read and assigned in rumtime as
attributes.

### Triangle

A trianagle wave is a ramp starting at 0, peaking at half the 

## Synopsis

```console
one-liner usage example
```

## Usage

Detailed usage and sample code for libraries, full help and options for tools.

## Installation

Installation instructions + requirements (should come automagically with
pyproject.toml)

```console
sample install session
```

## Support / Contributing

Issues can be opened on [Github](https://github.com/dickerdackel/FIXME/issues)

## Credits / Acknowledgements

* Thanks to [Make a README](https://www.makeareadme.com/)

## License

FIXME

This software is provided under the MIT license.

See LICENSE file for details.
