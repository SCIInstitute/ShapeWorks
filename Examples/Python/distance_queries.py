import pickle
from pathlib import Path
from tqdm import tqdm
from collections import Counter

filepath='distance_queries.txt'

splits = {}
curr_split = 0

with tqdm(total=Path(filepath).stat().st_size) as pbar:
    with open(filepath) as fd:
        line = fd.readline()
        while line:
            pbar.update(fd.tell() - pbar.n)
            if fd.tell() > 163364917:
                break

            if line.startswith('#'):
                _, command, n = line.split()
                if command == 'split':
                    curr_split = int(n)
                    splits[curr_split] = Counter()
                elif command == 'iter':
                    pass
            else:
                indices = [int(x) for x in line.split()[:6]]
                for i in range(3):
                    for j in range(3):
                        splits[curr_split][(indices[i], indices[j])] += 1

            line = fd.readline()

with open('stats.pickle', 'wb') as fd:
    pickle.dump(splits, fd)
