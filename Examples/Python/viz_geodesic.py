import os
import pygame 
from functools import lru_cache
import pylru

fname = '/scratch/karthik/projects/ShapeWorks/Examples/Python/Output/femur_mesh/shape_models/distance_queries.txt'
fd = open(fname)
N = 10982
DIM = 2000
total_lines = os.path.getsize(fname) // 10 # approx
total_iters = 500
lines_per_iter = total_lines / total_iters
cache_size = 16
cache = pylru.lrucache(cache_size)
cache_size_bytes = cache_size * N * 8
viz_queries_per_frame = 1000

pygame.init()
screen = pygame.display.set_mode([DIM, DIM])
running = True
font = pygame.font.SysFont('Inconsolata', 36)

n_lines = 0
cache_hits = 0
cache_misses = 0


while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
    screen.fill((255, 255, 255))

    for i in cache.keys():
        y = int((i/N) * DIM)
        pygame.draw.rect(screen, (200, 200, 255), (0, y, DIM, DIM//N))


    for _ in range(viz_queries_per_frame):
        line = fd.readline()
        n_lines += 1
        i, j = [int(x) for x in line.split()]

        c = (0, 0, 255)
        if i in cache:
            _ = cache[i] # force it to be "recently used"
            cache_hits += 1
        else:
            c = (255, 0, 0)
            cache_misses += 1
            cache[i] = 0.0

        # x,y corresponds to j,i because thats what rows/cols correspond
        # to in pygame coordinates
        y = int((i/N) * DIM)
        x = int((j/N) * DIM)
        pygame.draw.circle(screen, c, (x, y), c[0]//255*2+5)

    img = font.render(f'Number of acceses: {n_lines} / ~{total_lines}', True, (0, 0, 0))
    screen.blit(img, (20, 20))
    img = font.render(f'Approx number of iterations: {int(n_lines // lines_per_iter)} / {total_iters}', True, (0, 0, 0))
    screen.blit(img, (20, 50))
    img = font.render(f'Cache(size={cache_size}): {cache_hits:>9} hits | {cache_misses:>9} misses | {cache_misses / (cache_misses + cache_hits):.2} miss rate | ~{cache_size_bytes/2**20:.3} MB', True, (0, 0, 0))
    screen.blit(img, (20, 80))
    img = font.render(f'{viz_queries_per_frame} quer{"ies" if viz_queries_per_frame>1 else "y"} per frame', True, (0, 0, 0))
    screen.blit(img, (DIM - 400, 20))


    # Flip the display
    pygame.display.flip()

# Done! Time to quit.
pygame.quit()