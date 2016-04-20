# Laplacian Coating Transfer
## Digital Geometry Processing Final Project
### Evan Nickerson, Nathan Burrell

A project on transferring the surface details (the coating or "texture") from one mesh to another.

# The Theory
## Laplacian Coordinates
Laplacian coordinates can be simply described as the following:
The vector in R3 which describes the relative position of a vertex i based on its neighbours.

The Laplacian coordinates are most notably useful for preserving volume and features "as well as possible" under deformations and other constraints. In our project, we compute the Laplacian coordinate in order to capture high frequency details on the surface of one mesh and then transfer them to the surface of another.

### Boundaries
Boundaries have to be handled differently as the computation used for computing laplacian coordinates on a surface vertex does not work if it lies on a boundary. In this case we instead use a different method called "umbrella operator" in Implicit Mesh Fairing, Desbrun et al.

This method computes a laplacian coordinate based instead on the boundary vertex's position relative to its neighbouring boundary vertices, and ignores non-boundary vertices.

## Mapping From a Mesh S to a Mesh U
In order to transfer the surface details of a mesh S to a mesh U, we first have to create a mapping for each vertex in U to a vertex in S. As a generalized approach, we calculate the mapping using a UV mapping provided for each mesh. A vertex in U then exists on the UV map as a vertex i\_map, and likewise for S and j\_map. Then a vertex i of U is mapped to a vertex j of S, where j\_map is the closest vertex on the UV coordinates to i\_map.

In our project we mostly transferred details from a plane mesh to a smooth sphere using programmatically generated mappings, however, programs such as Blender or Maya allow you to export UV map data, which could be used instead.

## Smoothing
In order to capture the coating of a mesh, we compare the laplacian coordinates of the mesh (L\_s) to the laplacian coordinates of a smoothed copy of the mesh (L\_s'). Smoothing effectively destroys the details of the coating, and so when we compute the difference between the two:
L\_diff = L\_s - L\_s'.

L\_diff becomes the Laplacian coordinates of the mesh S which we use to apply the coating to U.

## Rotation and Application of the Laplacian Coordinates
In most cases, and in ours, the surface of S and U do not have consistently matching orientations. To solve this, when applying details from a vertex i of S to a vertex j of U, the L\_diff coordinates are rotated using a rotation matrix which rotates from the vertex normal of i to the vertex normal of j. In this way the Laplacian coordinates are re-oriented to the new mesh we are applying the coating to.

The applying the coating from vertex i of mesh S to vertex j of mesh U is as simple as:
j' = j + L\_diff_i

#The Code
## Build/Compilation Instructions:
This project is built in the same way as the lab code. See lab code instructions (they're more thorough than I would be).  

Dependencies:  
1. Eigen  
2. glfw  
3. opengl  

In order for the given file paths to work, the build folder should be in the same folder as the source directory "dgp-final". That is,  

-code  
--dgp-final  
--dgp-final-build  

### Windows/Visual Studio Error
When running in VS2015 on my home windows 7 computer, I experienced std::bad_alloc errors during the smoothing step, caused by the way visual studio and windows manages memory. The error is most often caused by running out of memory. These errors are not present when tested on the machines in ECS354 on OSX.

## main.cpp
main.cpp includes a method for loading, positioning, and rendering the meshes using OpenGP.

## Mapping.cpp/.h
The Mapping class provides static functions for generating different kinds of UV mappings. The UV mappings are returned as a SurfaceMesh::Vertex_property<Vec2\>.

## Mixer.cpp/.h
The main class for our algorithm. Takes a fromMesh, a toMesh, and both their UV mappings to return a copy of toMesh with the coating of fromMesh applied.

## Smoother.cpp/.h
The smoother used in the labs. One of the methods we used for getting the smoothed copy of S.

# Results
Up-to-date screenshots can be found in the screenshot folder of results.

## Cotan vs. Graph Laplacian Coordinates
Graph laplacian coordinates worked to an extent, however, on source meshes which have minute amounts of high-frequency detail (such as wool_e.obj) they failed to produce much of a result on the target mesh. This could be fixed by multiplying the laplacian coordinates of the source mesh by a scalar before applying them. However, doing so with the graph laplacian proved to result in excessive vertex drifting and poor results.

The cotangent laplacian solves this problem by not having the same vertex drift. Rather than being based on the average position of its neighbours and having a non surface normal aligned like the graph laplacian, the cotangent laplacian coordinates are based on a projection which results in no vertex drift. The difference between these results can be seen in graph\_laplacian20.png and cotan\_laplacian20.png.

## UV Mapping Method
The mapping method used produces good results on meshes which are high resolution with consistent edge lengths. However, having inconsistent vertex distribution and inconsistent edge lengths may result in some inconsistencies in appearance between the source mesh and the resulting applied coating.

As an example: a vertex in the middle of a large, flat face which is surrounded by steep edges would have to map to the closest vertex on one of those sharply-detailed edges, even though by the mesh's appearance, the original vertex lies firmly in the middle of a flatly-detailed plane. This could be solved by interpolating between vertices based on UV coordinates, however that method was left outside the scope of this project.

All in all, the UV mapping method provides a flexible and potentially customizable (through reading UV mapping data produced by blender/maya from a file) method for applying the coating to the target mesh.

## Smoothing
We used an iterative method of the explicit smoothing from the lab code. We attempted to implement the implicit smoothing included in the lab code as well, however the results produced by it were poor/bugged, and the source of the problem could not be pin-pointed.

The iterative explicit smoothing method succeeds in capturing high-frequency details, but fails to capture some of the more mid-frequency details we would like to see it capture.

## In Conclusion
Using the cotangent coordinates, the program succeeds in convincingly applying the coating of the source mesh to the target mesh. There were a few more features which could have been added such as reading UV mappings from a file or improved smoothing, however most of them would have proved to be relatively time-consuming to solve. In graphics, visual appearance is exceedingly important and on that front I believe the program now succeeds.