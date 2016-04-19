# Laplacian Coating Transfer
## Digital Geometry Processing Final Project
### Evan Nickerson, Nathan Burrell

A project on transferring the surface details (the coating or "texture") from one mesh to another.

# The Theory
## Laplacian Coordinates
Laplacian coordinates can be simply described as the following:
The vector in R3 which describes the offset of a vertex i from the average position of its neighbours.

The Laplacian coordinates are most notably useful for preserving volume and features "as well as possible" under deformations and other constraints. In our project, we compute the Laplacian coordinate in order to capture high frequency details on the surface of one mesh and then transfer them to the surface of another.

## Mapping From a Mesh S to a Mesh U
In order to transfer the surface details of a mesh S to a mesh U, we first have to create a mapping for each vertex in U to a vertex in S. As a generalized approach, we calculate the mapping using a UV mapping provided for each mesh. A vertex in U then exists on the UV map as a vertex i\_map, and likewise for S and j\_map. Then a vertex i of U is mapped to a vertex j of S, where j\_map is the closest vertex on the UV coordinates to i\_map.

In our project we mostly transferred details from a plane mesh to a smooth sphere using programmatically generated mappings, however, programs such as Blender or Maya allow you to export UV map data, which could be used instead.

## Smoothing
In order to capture the coating of a mesh, we compare the laplacian coordinates of the mesh (L\_s) to the laplacian coordinates of a smoothed copy of the mesh (L\_s'). Smoothing effectively destroys the details of the coating, and so when we compute the difference between the two: L\_diff = L\_s - L\_s'.

L\_diff becomes the Laplacian coordinates of the mesh S which we use to apply the coating to U.

## Rotation and Application of the Laplacian Coordinates
In most cases, and in ours, the surface of S and U do not have consistently matching orientations. To solve this, when applying details from a vertex i of S to a vertex j of U, the L\_diff coordinates are rotated using a rotation matrix which rotates from the vertex normal of i to the vertex normal of j. In this way the Laplacian coordinates are re-oriented to the new mesh we are applying the coating to.

The applying the coating from vertex i of mesh S to vertex j of mesh U is as simple as:
j' = j + L\_diff_i

#The Code
## main.cpp
main.cpp includes a method for loading, positioning, and rendering the meshes using OpenGP.

## Mapping.cpp/.h
The Mapping class provides static functions for generating different kinds of UV mappings. The UV mappings are returned as a SurfaceMesh::Vertex_property\<Vec2\>.

## Mixer.cpp/.h
The main class for our algorithm. Takes a fromMesh, a toMesh, and both their UV mappings to return a copy of toMesh with the coating of fromMesh applied.

## Smoother.cpp/.h
The smoother used in the labs. One of the methods we used for getting the smoothed copy of S.
