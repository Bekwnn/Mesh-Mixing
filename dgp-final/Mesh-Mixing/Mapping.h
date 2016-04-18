#include <OpenGP/SurfaceMesh/SurfaceMesh.h>

class Mapping {
    typedef OpenGP::SurfaceMesh::Vertex Vertex;

public:
    static void PlaneMapping(OpenGP::SurfaceMesh const& mesh, OpenGP::SurfaceMesh::Vertex_property<OpenGP::Vec2>& uvmapping);
    static void SphereMapping(OpenGP::SurfaceMesh const& mesh, OpenGP::SurfaceMesh::Vertex_property<OpenGP::Vec2>& uvmapping);
    static void SphereMappingMirrored(OpenGP::SurfaceMesh const& mesh, OpenGP::SurfaceMesh::Vertex_property<OpenGP::Vec2>& uvmapping);
    //returns true if uvmap is 0-1 for all values, false otherwise
    static bool IsUVMapGood(OpenGP::SurfaceMesh::Vertex_property<OpenGP::Vec2>& uvmapping);
};
