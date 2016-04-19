#include "Mapping.h"
#include <cmath>

#define PI 3.14159265358979
using namespace OpenGP;

/// Maps a mesh uv coords using x and z coordinates, ignores y (the world up) coordinate
void Mapping::PlaneMapping(SurfaceMesh const& mesh, SurfaceMesh::Vertex_property<Vec2>& uvmapping)
{
    Vec2 min(100,100), max(-100,-100);
    for (auto const& v : mesh.vertices())
    {
        if (mesh.position(v)[0] < min[0])
            min[0] = mesh.position(v)[0];
        else if (mesh.position(v)[0] > max[0])
            max[0] = mesh.position(v)[0];

        // y is world up axis for vec3
        if (mesh.position(v)[2] < min[1])
            min[1] = mesh.position(v)[2];
        else if (mesh.position(v)[2] > max[1])
            max[1] = mesh.position(v)[2];
    }


    for (Vertex v : mesh.vertices())
    {
        uvmapping[v] = Vec2((mesh.position(v)[0] - min[0])/(max[0]-min[0]), (mesh.position(v)[2] - min[1])/(max[1]-min[1]));
    }
}

/// Maps a mesh uv coords using spherical coordinates. Convert from (x,y,z) to (phi,theta,r)
/// then map to u and v using phi and theta
void Mapping::SphereMapping(SurfaceMesh const& mesh, SurfaceMesh::Vertex_property<Vec2>& uvmapping)
{
    for (auto const& vert : mesh.vertices())
    {
        Vec3 p = mesh.position(vert);

        float u = 0.5 + (atan2f(p[2],p[0])/(2*PI));
        float v = 0.5 - (asinf(p[1])/PI);

        uvmapping[vert] = Vec2(u, v);
    }

}

/// Maps a mesh uv coords using spherical coordinates. Convert from (x,y,z) to (phi,theta,r)
/// then map to u and v using phi and theta
void Mapping::SphereMappingMirrored(SurfaceMesh const& mesh, SurfaceMesh::Vertex_property<Vec2>& uvmapping)
{
    for (auto const& vert : mesh.vertices())
    {
        Vec3 p = mesh.position(vert);

        float u = 0.5f + (atan2f(p[2],p[0])/(2*PI));
        float v = 0.5f - (asinf(p[1])/PI);

        u *= 2.0f;

        if (u > 1.0f)
        {
            u = 1.0f - (u - 1.0f);
        }

        uvmapping[vert] = Vec2(u, v);
    }

}

void Mapping::HemisphereMapping(SurfaceMesh const& mesh, SurfaceMesh::Vertex_property<Vec2>& uvmapping)
{
    for (auto const& vert : mesh.vertices())
    {
        Vec3 p = mesh.position(vert);

        float u = 2.0f * (0.5f + (atan2f(p[2],p[0])/(2.0f*PI)));
        float v = 2.0f * (0.5f + (asinf(p[1])/PI));

        uvmapping[vert] = Vec2(u,v);
    }
}

/// Returns true if all cordinates in a uvmapping are from 0.f to 1.f
bool Mapping::IsUVMapGood(SurfaceMesh::Vertex_property<Vec2>& uvmapping)
{
    bool flag = true;
    for (auto vec : uvmapping.vector())
    {
        if (vec[0] < 0.f || vec[0] > 1.f || vec[1] < 0.f || vec[1] > 1.f)
        {
            flag = false;
        }
    }
    return flag;
}
