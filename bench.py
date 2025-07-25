from statistics import mean, median, stdev
from timeit import timeit

from lfo import LFO

def bench(*fns, runs=5, number=1_000_000):
    for fn in fns:
        print(fn)
        print('-' * 72)
        timings = []
        for i in range(runs):
            timings.append(timeit(fn, number=number))
            print(f'{i}: {timings[-1]}')

        print(f'{mean(timings)=:.5f}  {median(timings)=:.5f}  {stdev(timings)=:.5f}')
        print(f'{(number / mean(timings)) / 60:.5f}  {(number / median(timings)) / 60:.5f}')
        print()


def main():
    lfo = LFO()
    def f0(): LFO()
    def f1(): lfo.sine
    def f2(): lfo.cosine
    def f3(): lfo.triangle
    def f4(): lfo.sawtooth
    def f5(): lfo.square
    def f6(): lfo.inv_sine
    def f7(): lfo.inv_cosine
    def f8(): lfo.inv_triangle
    def f9(): lfo.inv_sawtooth
    def f10(): lfo.inv_square
    def f11(): lfo.t
    def f12(): lfo.normalized
    bench(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, number=10_000_000)

if __name__ == "__main__":
    main()
