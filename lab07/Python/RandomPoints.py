import random
import collections

N = 500
OUT_FILE = "../points.dat"
OUT_DATA = []
BOUNDS = (1000, 1000)

for i in range(N):
    OUT_DATA.append((random.randrange(-BOUNDS[0], BOUNDS[1]), random.randrange(-BOUNDS[0], BOUNDS[1])))

print([item for item, count in collections.Counter(OUT_DATA).items() if count > 1])

file = open(OUT_FILE, "w+")
file.write(f"{len(OUT_DATA)}\n")
file.write('\n'.join(f"{x[0]/100}\t{x[1]/100}" for x in OUT_DATA))
