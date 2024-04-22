# ----------------------- ACTUAL CODE -----------------------
import math
import sys

from PIL import Image
import numpy as np

r = 20

triangle_height = 1

points = []
#mask = Image.open("Hard_MASK.png")  # Image. For any given points P(x, y) if mask[x, y] is black
# that means the point P is inside the shape

# points_x_set = dict()

#size_x = mask.size[0]
#size_y = mask.size[1]
#mask = mask.load()

points_list = []

points_file = open("sorted_points.dat")
points_file.readline()
for l in points_file.readlines():
    points.append(np.array([float(l.split("\t")[0]), float(l.split("\t")[1])]))
    points_list.append((float(l.split("\t")[0]), float(l.split("\t")[1])))

edges = []
edges_file = open("sorted_edges.es")
edges_file.readline()
for l in edges_file.readlines():
    edges.append((int(l.split("\t")[0]), int(l.split("\t")[1])))

front = edges.copy()

mesh_points = points.copy()
mesh_elements = []


# build sorted set for faster search
# for p in points:
#     if p[0] in points_x_set:
#         points_x_set[p[0]].append(p[1])
#     else:
#         points_x_set[p[0]] = [p[1]]


def ccw(A, B, C):
    return (((C[1] - A[1]) * (B[0] - A[0])) > ((B[1] - A[1]) * (C[0] - A[0])))


# returns true if the line from A->B intersects with C->D
def intersect(A, B, C, D):
    return ccw(A, C, D) != ccw(B, C, D) and ccw(A, B, C) != ccw(A, B, D)

eps = 0.1
# checks if point c is on the line created by ab
def isPointOnLine(a, b, c):
    return -eps < (np.linalg.norm(a - c) + np.linalg.norm(c - b) - np.linalg.norm(a - b)) < eps


def CrossesFront(p1, pi):
    #front_points = 1 if edges[idx][0] < base_edge_count else 0
    #front_points += 1 if edges[idx][1] < base_edge_count else 0
    for (i, e) in enumerate(edges):
        if i == idx:
            continue

        start, end = mesh_points[e[0]], mesh_points[e[1]]

        if (not (start == A).all() and
                not (end == A).all() and
                not (start == B).all() and
                not (end == B).all() and
                intersect(A, B, start, end)):
            return True

        if not (start == A).all() and not (end == A).all() and e[0] != pi and e[1] != pi and intersect(A, p1, start, end):
            return True

        if not (start == B).all() and not (end == B).all() and e[0] != pi and e[1] != pi and intersect(B, p1, start, end):
            return True

    for (i, e) in enumerate(mesh_elements):
        last_p = mesh_points[e[2]]
        for (j, p) in enumerate(e):
            if (mesh_points[p] == p1).all():
                #last_p = mesh_points[p]
                continue
            if not (last_p == A).all() and not (mesh_points[p] == A).all() and intersect(p1, A, last_p, mesh_points[p]):
                return True
            if not (last_p == B).all() and not (mesh_points[p] == B).all() and intersect(p1, B, last_p, mesh_points[p]):
                return True
            if not (mesh_points[p] == A).all() and isPointOnLine(A, p1, mesh_points[p]):
                return True
            if not (mesh_points[p] == B).all() and isPointOnLine(B, p1, mesh_points[p]):
                return True
            # if (edges[idx][0] < base_edge_count and edges[idx][1] >= base_edge_count and 0 < pi < base_edge_count and abs(pi - edges[idx][0]) < 3):
            #     ApX_ccw = ccw(A, mesh_points[p], mesh_points[edges[(idx + 1) % len(edges)][1]])
            #     ABX_ccw = ccw(A, B, mesh_points[edges[(idx + 1) % len(edges)][1]])
            #     if not (mesh_points[p] == A).all() and not (mesh_points[p] == B).all() and not (mesh_points[p] == p1).all() and ApX_ccw != ABX_ccw:
            #         return True
            # print(f"px -> {ApX_ccw}")
            # print(f"bx -> {ABX_ccw}")
            last_p = mesh_points[p]
    return False

    # for (i, e) in enumerate(edges):
    #     for (j, p) in enumerate(e):

def sign (p1, p2, p3):
    return (p1[0] - p3[0]) * (p2[1] - p3[1]) - (p2[0] - p3[0]) * (p1[1] - p3[1])

# O(n)
def IsInsideMesh(p1):
    for (i, e) in enumerate(mesh_elements):
        d1 = sign(p1, mesh_points[e[0]], mesh_points[e[1]])
        d2 = sign(p1, mesh_points[e[1]], mesh_points[e[2]])
        d3 = sign(p1, mesh_points[e[2]], mesh_points[e[0]])

        has_neg = (d1 < 0) or (d2 < 0) or (d3 < 0)
        has_pos = (d1 > 0) or (d2 > 0) or (d3 > 0)

        if not (has_neg and has_pos):
            return True

    return False

def GetPointsAngle(p, q, r):
    val = (q[1] - p[1]) * (r[0] - q[0]) - (q[0] - p[0]) * (r[1] - q[1])

    return val

def PointInsidePolygon(p1,poly):
    n = len(poly)
    x, y = p1
    inside = np.zeros(1,np.bool_)
    p2x = 0.0
    p2y = 0.0
    xints = 0.0
    p1x,p1y = poly[0]
    for i in range(n+1):
        p2x,p2y = poly[i % n]
        idx = np.nonzero((y > min(p1y,p2y)) & (y <= max(p1y,p2y)) & (x <= max(p1x,p2x)))[0]
        if p1y != p2y:
            xints = (y-p1y)*(p2x-p1x)/(p2y-p1y)+p1x
        if p1x == p2x:
            inside[idx] = ~inside[idx]
        else:
            idxx = idx[x <= xints]
            inside[idxx] = ~inside[idxx]

        p1x,p1y = p2x,p2y
    return inside

def IsLineOccupied(p1):
    tri_count_A = 0
    tri_count_B = 0
    # is_A_on_front = edges[idx][0] < base_edge_count and p1 < base_edge_count and abs(edges[idx][0] - p1) == 1
    # is_B_on_front = edges[idx][1] < base_edge_count and p1 < base_edge_count and abs(edges[idx][1] - p1) == 1

    is_A_on_front = edges[idx][0] < base_edge_count and p1 < base_edge_count and (
                abs(edges[idx][0] - p1) == 1 or abs(edges[idx][0] - p1) == mod_n_1)
    if is_A_on_front:
        is_A_on_front = any((e[0] == edges[idx][0] and e[1] == p1) or \
                            (e[0] == p1 and e[1] == edges[idx][0]) for e in front)
    is_B_on_front = edges[idx][1] < base_edge_count and p1 < base_edge_count and (
                abs(edges[idx][1] - p1) == 1 or abs(edges[idx][1] - p1) == mod_n_1)
    if is_B_on_front:
        is_B_on_front = any((e[0] == edges[idx][1] and e[1] == p1) or \
                            (e[0] == p1 and e[1] == edges[idx][1]) for e in front)

    for (i, e) in enumerate(mesh_elements):
        if edges[idx][0] in e and p1 in e:
            tri_count_A += 2 if is_A_on_front else 1
        if edges[idx][1] in e and p1 in e:
            tri_count_B += 2 if is_B_on_front else 1
        if tri_count_A >= 2 or tri_count_B >= 2:
            return True

        #check if are exactly the same
        if edges[idx][0] in e and edges[idx][1] in e and p1 in e:
            return True

    return False

#  Returns the closes point to C, if not found, returns C
def CheckPointProximity(C, radius=r) -> (np.ndarray, int, float):
    best_proxim = 1000000
    best_point = -1
    do_front_check = edges[idx][0] < base_edge_count or edges[idx][1] < base_edge_count

    # Missing a collinearity check!
    for (i, p) in enumerate(mesh_points):
        # Skip the points if p is equal to one of the triangle base points
        if (p == A).all() or (p == B).all():
            continue

        dist = np.linalg.norm(p - C)
        if dist > radius or dist >= best_proxim:
            continue

        if CrossesFront(p, i):
            continue

        if IsLineOccupied(i):
            continue
        # and i < base_edge_count and (np.cross(A, p) * np.cross(A, B) >= 0) and (np.cross(B, p) * np.cross(B, A) >= 0):
        if do_front_check:
            m1 = (p + A) / 2
            m2 = (p + B) / 2
            c1 = PointInsidePolygon(m1, points) or (edges[idx][0] < base_edge_count and i < base_edge_count and (
                                                abs(edges[idx][0] - i) == 1 or abs(edges[idx][0] - i) == mod_n_1))
            c2 = PointInsidePolygon(m2, points) or (edges[idx][1] < base_edge_count and i < base_edge_count and (
                                                abs(edges[idx][1] - i) == 1 or abs(edges[idx][1] - i) == mod_n_1))
            is_inside = c1 and c2
            if not is_inside:
                continue

        best_proxim = dist
        best_point = i
            # return p, i
    # for i in range(-r, r + 1):
    #     if np.round(C[0] + i) not in points_x_set:
    #         continue
    #     for y in points_x_set[np.round(C[0] + i)]:
    #         if np.linalg.norm([i, C[1] - y]) < r:
    #             return C[0] + i, y

    return C if best_point == -1 else mesh_points[best_point], best_point, best_proxim


r90_mx = np.array([[0, -1], [1, 0]])
rn90_mx = np.array([[0, 1], [-1, 0]])
idx = 0
A = mesh_points[edges[0][0]]
B = mesh_points[edges[0][1]]

base_edge_count = len(edges)
mod_n_1 = base_edge_count - 1

while not np.array_equal(A, B) and idx < len(edges):
    # Check if current points are not already building some other triangle
    # If AB is part of 2 or more triangles (or 1 triangle and the original front)
    # then you cannot create more triangles using it
    # -- Check 2 triangles --
    tri_count = 0
    is_front = edges[idx][0] < base_edge_count and edges[idx][1] < base_edge_count and (
                abs(edges[idx][0] - edges[idx][1]) == 1 or abs(edges[idx][0] - edges[idx][1]) == mod_n_1)

    for (i, e) in enumerate(mesh_elements):
        if edges[idx][0] in e and edges[idx][1] in e:
            tri_count += 2 if is_front else 1
            if tri_count >= 2:
                print(f"Duplicate [{round(idx / len(edges), 2) * 100}% done]")
                idx += 1
                break
    if tri_count >= 2:
        continue


    A = mesh_points[edges[idx][0]]
    B = mesh_points[edges[idx][1]]

    #dir_v = B - A  # (next_p[0] - p[0], next_p[1] - p[1])
    # Rotate counter-clockwise
    #dir_v = np.array([-dir_v[1], dir_v[0]]) * triangle_height
    mid_point = (A + B) / 2
    O = (A - mid_point) * 3 ** 0.5
    C = mid_point + O @ r90_mx
    #C = GetThirdTriPoint(A, B, 1) #dir_v + mid_point  # third point making up the triangle

    # Check for nodes in close proximity
    merged_C = CheckPointProximity(C)
    was_merged = not merged_C[1] == -1
    C = merged_C[0]

    # check if is inside the shape
    #if not was_merged and size_x > C[0] > 0 and size_y > C[1] > 0 and mask[C[0], C[1]][0] > 10:
    #if not was_merged and (not 0 > C[0] > 1000 and not 0 > C[1] > 1000): # square
    #if not was_merged and (np.linalg.norm(C) > 1000 or IsInsideMesh(C)): # circle
    if not was_merged and not PointInsidePolygon(C, points):
        # if not then do the same but with clockwise rotation
        C_rot = mid_point + O @ rn90_mx
        if not PointInsidePolygon(C_rot, points):
            # If the flipped point is still not inside then choose close one of the two
            rotated_C = CheckPointProximity(C_rot, 99999)
            merged_C = CheckPointProximity(C, 99999)

            # I'm lost, you just cant create a triangle here...
            if rotated_C[1] == -1 and merged_C[1] == -1:
                print(f"Skipping a triangle (Bounds) [idx: {idx}, edges: {edges[idx]}]")
                idx += 1
                continue

            # print(f"rotated: {GetPointsAngle(A, rotated_C[0], B)}")
            # print(f"merged: {GetPointsAngle(A, merged_C[0], B)}")
            # print(f"poly: {GetPointsAngle(A, B, mesh_points[edges[idx+1][1]])}")
            merged_C = merged_C if merged_C[2] < rotated_C[2] else rotated_C
        else:
            merged_C = CheckPointProximity(C_rot)
        was_merged = not merged_C[1] == -1
        C = merged_C[0]

    if not was_merged and IsInsideMesh(C):
        merged_C = CheckPointProximity(C, 99999)
        if merged_C[1] == -1:
            print(f"Skipping a triangle (Mesh) [idx: {idx}, edges: {edges[idx]}]")
            idx += 1
            continue
        was_merged = not merged_C[1] == -1
        C = merged_C[0]

    if not was_merged and CrossesFront(C, -1):
        merged_C = CheckPointProximity(C, 99999)
        if merged_C[1] == -1:
            idx += 1
            print(f"Skipping a triangle (Front Cross) [idx: {idx}, edges: {edges[idx]}]")
            continue
        was_merged = not merged_C[1] == -1
        C = merged_C[0]
        print(f"Crosses Front!, idx = {idx}, edges: {edges[idx]}")

    C_idx = merged_C[1] if was_merged else len(mesh_points)
    mesh_elements.append((edges[idx][0], C_idx, edges[idx][1]))

    # Add new triangle edges
    edges.append((edges[idx][0], C_idx))
    edges.append((C_idx, edges[idx][1]))

    # Delete old (front) edges
    # del edges[idx-1]
    # edges[idx] = (-1, -1)


    if not was_merged:
        mesh_points.append(C)

    # Check if all edges construct at least 2 triangles
    # edges_freq = {}
    # for (i, edge) in enumerate(edges):
    #     for (j, elem) in enumerate(mesh_elements):
    #         if edge[0] in elem and edge[1] in elem:
    #             if edge in edges_freq:
    #                 edges_freq[edge] += 1
    #             else:
    #                 edges_freq[edge] = 1
    #
    # should_exit = True
    # for (i, f) in enumerate(edges_freq.values()):
    #     if f < 2:
    #         should_exit = False
    #         break
    # if should_exit:
    #     break


    idx += 1
    # p = next_p
    # next_p = mesh_points[idx % len(mesh_points)]

# mesh_points.append(p)
# #edge_idx+=1
# new_edges.append((edge_idx, edge_idx+1))
# mesh_points.append(next_p)
# new_edges.append((edge_idx + 1, 0))
# new_edges.append((edge_idx, 0))

import matplotlib.pyplot as plt


x = [e[0] for e in mesh_points]
y = [-e[1] for e in mesh_points]

plt.figure()
plt.gca().set_aspect('equal')
#faces_colors = [mesh_points[x[0]][0]/1000 for x in mesh_elements] # Kolorki jakieś, możesz się tym pobawić
faces_colors = [1 for x in mesh_elements] # stały kolor
plt.tripcolor(x,y, mesh_elements, facecolors=faces_colors, shading='flat', edgecolors='k')
plt.colorbar()
plt.title('Duszek')

plt.show()


out_file = open("mesh_points.dat", mode="w+")
out_file.write(f'{str(len(mesh_points))}\n')
out_file.write('\n'.join(f'{x[0]}\t{x[1]}' for x in mesh_points))
out_file.close()

out_file = open("mesh_elements.dat", mode="w+")
out_file.write(f'{str(len(mesh_elements))}\n')
out_file.write('\n'.join(f'{x[0]}\t{x[1]}\t{x[2]}' for x in mesh_elements))
out_file.close()

# out_file = open("mesh_edges.dat", mode="w+")
# out_file.write(f'{str(len(new_edges))}\n')
# out_file.write('\n'.join(f'{x[0]}\t{x[1]}' for x in new_edges))
