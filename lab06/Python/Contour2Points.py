import numpy as np
from PIL import Image, ImageOps
import cv2 as cv
import random
import sys
import math

THRESHOLD = 8  # 0 -255
DISTANCE = 40

# points from 0 to 2pi used to get points on a circle later
angles = np.arange(0, 2 * math.pi * DISTANCE, dtype=float) / DISTANCE

print(angles)

size = [1, 1]

#img = cv.imread(sys.argv[1] if len(sys.argv) > 1 else 'Hard.png', flags=cv.IMREAD_GRAYSCALE)
im = Image.open("Outline_trudny_wariant.png")
im = ImageOps.grayscale(im)

img = im.load()

size_x = im.size[0]
size_y = im.size[1]

points = []
edges = []

#size_multip = [size_x / size[0], size_y / size[1]]

p = (0, 0)

def ArePointsClose(p1: np.array, p2: np.array, max_dist) -> bool:
    R = max_dist
    max_dist *= 2
    if abs(p1[0] - p2[0]) > max_dist or abs(p1[1] - p2[1]) > max_dist:
        return False
    return False if np.linalg.norm(p1 - p2) > R else True

# Finds the best next point while considering the direction between the last 2 points
# (to keep the line going in the same direction)
def FindNextBestPoint(point: np.array, dir: np.array) -> np.array:
    best_rating = -1000000
    best_point = point
    # Normalize dir
    dir = dir / np.linalg.norm(dir)
    for a in angles:
        # Point on the circle given angle "a"
        cp = np.array([round(DISTANCE * math.sin(a) + point[0] + 0.5), round(DISTANCE * math.cos(a) + point[1] - 0.5)])
        if cp[0] < 0 or cp[0] >= size_x:
            continue
        if cp[1] < 0 or cp[1] >= size_y:
            continue
        new_dir = cp - point
        new_dir = new_dir / np.linalg.norm(new_dir)  # normalize
        dot = (np.dot(new_dir, dir) + 1) * 10
        # if not img[cp[0], cp[1]] == 255:
        #     print(f'point {cp}, color: {img[cp[0], cp[1]]}')
        rating = dot * (255 - img[cp[0], cp[1]])
        if rating > best_rating:
            best_rating = rating
            best_point = cp

    #print(f"best point: {best_point}")
    return best_point


next_p = FindNextBestPoint(p, np.array([0.7, 0.7235]))
dir_v = next_p - p
edge_idx = -1
idx = 0

# Find Holes
for i in range(0, size_x):
    for j in range(0, size_y):
        if img[i, j] < THRESHOLD:
            best_dist = 10000
            for p in points:
                if ((i, j) == p).all():
                    print("already seen this point")
                    best_dist = 0
                    break
                dist = np.linalg.norm((i, j) - p)
                best_dist = min(dist, best_dist)
                if best_dist < DISTANCE:
                    break
            if best_dist >= DISTANCE:
                print(f"found new starting point at {i}, {j}")
                p = np.array([i, j])
                p_start = p
                next_p = FindNextBestPoint(p, np.array([0.7, 0.7235]))
                points.append(p)
                points.append(next_p)
                edge_idx += 1
                edge_start = edge_idx
                edges.append((edge_idx, edge_idx+1))
                dir_v = next_p - p
                idx = 0
                while (idx <= 2 or not ArePointsClose(p_start, next_p, DISTANCE)) and idx < 10000:
                    new_p = FindNextBestPoint(next_p, dir_v)
                    points.append(new_p)
                    dir_v = new_p - next_p
                    p = next_p
                    next_p = new_p
                    idx += 1
                    edge_idx+=1
                    edges.append((edge_idx, edge_idx + 1))
                edges.append((edge_idx + 1, edge_start))
                edge_idx+=1
                print("Finished loop")

out_file = open("sorted_points.dat", mode="w+")
out_file.write(f'{str(len(points))}\n')
out_file.write('\n'.join(f'{x[0]}\t{x[1]}' for x in points))


out_file = open("sorted_edges.es", mode="w+")
out_file.write(f'{str(len(edges))}\n')
out_file.write('\n'.join(f'{x[0]}\t{x[1]}' for x in edges))

# out_file = open("sorted_edges.es", mode="w+")
# out_file.write(f'{str(len(points))}\n')
# out_file.write('\n'.join(f'{x}\t{(x+1) % len(points)}' for x in range(len(points))))

#print(f"next: {next_p}, p: {p}")
#print(''.join(f'({x[0]}, {size_y-x[1]}),' for x in points)[:-1]) # FLIP POINTS