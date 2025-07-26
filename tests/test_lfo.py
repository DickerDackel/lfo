from pytest import approx
from time import sleep

from lfo import LFO

def test_defaults():
    l = LFO()

    assert l.sine_attenuverter == 1.0
    assert l.sine_offset == 0.0

    assert l.cosine_attenuverter == 1.0
    assert l.cosine_offset == 0.0

    assert l.triangle_attenuverter == 1.0
    assert l.triangle_offset == 0.0

    assert l.sawtooth_attenuverter == 1.0
    assert l.sawtooth_offset == 0.0

    assert l.square_attenuverter == 1.0
    assert l.square_offset == 0.0
    assert l.pw == 0.5
    assert l.pw_offset == 0.0

    assert l.one_attenuverter == 1.0
    assert l.one_offset == 0.0

    assert l.zero_attenuverter == 1.0
    assert l.zero_offset == 0.0

    assert l.cycle == 0

    # assert approx(lt(), 0.01) == approx(lt(), 0.01)


if __name__ == '__main__':
    ...
