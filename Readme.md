# Computational Geometry
### This is a repo with all the *mini-projects* I wrote for my university class "*Computational Geometry*"

## About the code
Everything You see when running the code is C++, but in some cases I had to use Python, mostly to extract a list of points from an image. The UI is in Polish,
but the code itself is (mostly) kept in English.<br>
Let's be real, not everything is commented, because, as we all know, "a good code doesn't need comments"...<br>
I do not plan on ever updating this repo with new features, because all the *features* are just exercises I had to do.

## How is it made?
I made it on OpenGL (for cross-platform support) and glfw.
For the GUI library I used a trusted, fast and responsive [ImGui](https://github.com/ocornut/imgui).<br>
For anyone trying to do these kind of things, just to pass the class, I would recommend very different ways from mine, mostly javascript or python.<br>
This is not what I wanted to do, so I spent more time on every project in hopes I'll learn something (I did).

### This repo consists of 7 parts (and a final project (different repo)), each is called lab0X. Here's a more detailed list of them:
1. [Straight line (equation, translation, reflection, etc.), Point representation.](#some-basic-lines-and-points-interactions)
2. [- Continuation - Triangle from 2 lines](#3-lines-given-by-equations-forming-a-triangle)
3. [Polygon / Triangle (area / hit checks)](#triangle-area)
4. [Convex hull, collision detection](#convex-hull)
5. [1D/2D Range Trees](#2d-range-tree-sorting)
6. [Advancing Front / Mesh Generation](#advancing-front-algorithm-mesh-generation)
7. [Delaunay and it's *derivatives*](#base-delaunay-mesh-generation)
8. [Final Project (*Team*-Project)](https://github.com/AndyFilter/VoronoiCityAnalysis)

# Detailed Showcases / Descriptions
<details open>
  <summary>Lab 1 and 2</summary>

### Some basic lines and points interactions
![points intersection](https://github.com/AndyFilter/Geometria-Obliczeniowa/assets/69699046/1ebf1693-1d22-4425-a28c-881f3da2ed57)

### 3 lines given by equations forming a triangle
![triangle from 3 lines](https://github.com/AndyFilter/Geometria-Obliczeniowa/assets/69699046/644596c2-0dac-4f3d-99b6-8e3fae934c05)


*More stuff that is just too simple / boring to show off is in the program itself**
</details>

<details open>
  <summary>Lab 3</summary>

### Triangle Area
![points intersection](Media/tri_area.gif)

### Triangle-Point Check
![Is a given point inside a triangle](Media/tri_point_check.gif)

### Polygon-Point Check
![Is a given point inside a polygon](Media/poly_point_check.gif)

*Again, there is a bit more here, it's just pretty boring**
</details>

<details open>
  <summary>Lab 4</summary>

### Convex Hull
![convex hull](https://github.com/AndyFilter/Geometria-Obliczeniowa/assets/69699046/25eff14b-b772-41d6-b82f-33a0967f1db6)

### Starfleet (game)
![a silly little game](Media/starfleet.gif)

</details>

<details open>
  <summary>Lab 5</summary>

### 2D Range Tree (*sorting*)
![Range tree being played with](Media/2D_range_tree.gif)

*When You hover over a node it shows You the y-axis of that subtree*

</details>

<details open>
  <summary>Lab 6</summary>

### Advancing Front Algorithm (mesh generation)
![Amongus being created before Your own eyes](Media/amongus_creation.gif)

*There also is a way to export and import generated meshes**

</details>

<details open>
  <summary>Lab 7</summary>

### Base Delaunay mesh *generation*
![Base delaunay](Media/delaunay_base_amongus.gif)

### Weighted + Smoothed Delaunay mesh *generation*
![Base delaunay](Media/delaunay_weighted_apple.gif)

</details>

# How to run
The code is adapted to run on Windows and Linux. I was mostly running it on Linux, but I checked all the sub-projects afterwards on Windows.<br>
It's not a Visual Studio project! It's just a cmake project that I coded in IDE that I like :)<br>
To run, just use cmake or an IDE that You like :] (that supports cmake).
When it comes to the couple of python scripts scattered across all folders, You'll just have to install all the dependencies manually,
but there are not many of them, and the ones I chose to use are very common ones.

# Licence
Feel free to use it in educational purposes, if You think some of my visualizations can help convey how a given algorithm works, then use it.
Record a video, a gif, do a live presentation. Whatever helps people understand the beauty of **Computational Geometry <3**

# Hope it helps!
Or at least You enjoyed looking at the animated gifs...

