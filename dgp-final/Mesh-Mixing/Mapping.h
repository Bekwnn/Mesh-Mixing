#include <OpenGP/SurfaceMesh/SurfaceMesh.h>

class Mapping {
    typedef OpenGP::SurfaceMesh::Vertex Vertex;

public:
    static void PlaneMapping(OpenGP::SurfaceMesh const& mesh, OpenGP::SurfaceMesh::Vertex_property<OpenGP::Vec2>& uvmapping);
    static void SphereMapping(OpenGP::SurfaceMesh const& mesh, OpenGP::SurfaceMesh::Vertex_property<OpenGP::Vec2>& uvmapping);
};
