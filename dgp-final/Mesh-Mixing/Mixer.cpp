#include "Mixer.h"
#include "Mapping.h"
using namespace OpenGP;

SurfaceMesh Mixer::ApplyCoating(SurfaceMesh const& meshFrom, SurfaceMesh const& meshTo,
                                SurfaceMesh::Vertex_property<Vec2> meshFromMap,
                                SurfaceMesh::Vertex_property<Vec2> meshToMap)
{
    /* TODO:
     *  - use the uv coordinates to map vertices from one mesh to the other
     *  - set the result map to be equal to the meshTo mesh
     *  - capture the high freq details of meshFrom (S) by diffing it with a smoothed version of itself (S*)
     *  - encode the coating based on laplacian coordinates L (of S) and L* (of S*) then encoding of vertex i is L - L*
     *  - meshTo (U). Align S to U using surface normals of S* and U* (in our case U is a sphere and U = U*)
     *  - add L - L* to the laplacian of U, with respect to rotation alignment R for a vertex j where i maps to j
     *  - use the new laplacian to get the new vertex position
     *  - ???
     *  - profit!
     */

    std::map<SurfaceMesh::Vertex, SurfaceMesh::Vertex> vertexMappingStoU;

    for (auto vertS : meshFrom.vertices())
    {
        Vec2 S = meshFromMap[vertS];
        SurfaceMesh::Vertex closest;
        float bestDist = 100.f;
        for (auto vertU : meshTo.vertices())
        {
            Vec2 U = meshToMap[vertU];
            float udiff = S[0]-U[0];
            float vdiff = S[1]-U[1];

            float vertDist = fabs(sqrt(udiff*udiff + vdiff*vdiff));
            if (vertDist > bestDist)
                closest = vertU;
        }

        //map(vertS, closest)
        vertexMappingStoU.insert(std::make_pair(vertS, closest));
    }

    //RETURN FINAL RESULT
    return SurfaceMesh();
}
