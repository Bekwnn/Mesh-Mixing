#include <OpenGP/SurfaceMesh/SurfaceMesh.h>

class Mixer {
    typedef OpenGP::SurfaceMesh SurfaceMesh;
    typedef OpenGP::Vec2 Vec2;

public:
    //using passed uvmappings to map S to U for now.
    static SurfaceMesh ApplyCoating(SurfaceMesh const &meshFrom, SurfaceMesh const& meshTo,
                                    SurfaceMesh::Vertex_property<Vec2> meshFromMap,
                                    SurfaceMesh::Vertex_property<Vec2> meshToMap);

private:
    static std::map<SurfaceMesh::Vertex, SurfaceMesh::Vertex> MapUVs(SurfaceMesh const &meshFrom, SurfaceMesh const& meshTo,
                                                                     SurfaceMesh::Vertex_property<Vec2> meshFromMap,
                                                                     SurfaceMesh::Vertex_property<Vec2> meshToMap);
};
