#include "Mapping.h"
#include <cmath>

#define PI 3.14159265358979
using namespace OpenGP;

void Mapping::PlaneMapping(SurfaceMesh const& mesh, SurfaceMesh::Vertex_property<Vec2>& uvmapping)
{
    //TODO: incomplete
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

void Mapping::SphereMapping(SurfaceMesh const& mesh, SurfaceMesh::Vertex_property<Vec2>& uvmapping)
{
    //TODO: incomplete
    /* Maps a mesh uv coords using spherical coordinates. Convert from (x,y,z) to (phi,theta,r)
     * then map to u and v using phi and theta
     */
    for (auto const& v : mesh.vertices())
    {
        Vec3 p = mesh.position(v);
        float theta = acosf(p[1] / sqrtf(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]));
        float phi = atan(p[2] / p[0]);

        // Convert theta and phi -> [0,1]
        theta = (theta + PI) / (2.0f*PI);
        phi = (phi + (PI/2.0f)) / PI;
        theta = fmin(1.0f, fmax(0.0f, theta));
        phi = fmin(1.0f, fmax(0.0f, phi));

        uvmapping[v] = Vec2(theta, phi);
    }

}

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
