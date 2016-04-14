#include "Mixer.h"
#include "Mapping.h"
#include "internal/SurfaceMeshVerticesKDTree.h"
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

    SurfaceMesh resultMesh;

    std::map<SurfaceMesh::Vertex, SurfaceMesh::Vertex> vertexMappingStoU
            = MapUVs(meshFrom, meshTo, meshFromMap, meshToMap);

    return SurfaceMesh();
}

std::map<SurfaceMesh::Vertex, SurfaceMesh::Vertex> Mixer::MapUVs(SurfaceMesh const& meshFrom, SurfaceMesh const& meshTo,
                                                                 SurfaceMesh::Vertex_property<Vec2> meshFromMap,
                                                                 SurfaceMesh::Vertex_property<Vec2> meshToMap)
{
    std::map<SurfaceMesh::Vertex, SurfaceMesh::Vertex> vertexMappingStoU;

    std::vector<std::pair<SurfaceMesh::Vertex, SurfaceMesh::Vertex>> vertexMapping;

    SurfaceMesh flatU(meshTo);
    for (auto vertU : meshTo.vertices())
    {
        Vec2 tempv2 = meshToMap[vertU];
        flatU.position(vertU) = Vec3(tempv2[0], tempv2[1], 0);
    }

    SurfaceMeshVerticesKDTree meshToTree(flatU);

    size_t vertexID = 0;
    for (auto vertS : meshFrom.vertices())
    {
        Vec2 S = meshFromMap[vertS];
        vertexMapping.push_back( std::make_pair(vertS, SurfaceMesh::Vertex( meshToTree.closest_vertex( Vec3(S[0], S[1], 0)).idx() ) ) );

        vertexID++;
        if (vertexID % 100 == 0)
            std::cout << "progress: " << vertexID << std::endl;
    }

    vertexMappingStoU.insert(begin(vertexMapping), end(vertexMapping));

    return vertexMappingStoU;
}
