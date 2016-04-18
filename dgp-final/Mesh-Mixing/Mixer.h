#include <OpenGP/SurfaceMesh/SurfaceMesh.h>

class Mixer {
    typedef OpenGP::SurfaceMesh SurfaceMesh;
    typedef OpenGP::Vec2 Vec2;
    typedef OpenGP::Vec3 Vec3;

public:
    //using passed uvmappings to map S to U for now.
    static SurfaceMesh ApplyCoating(SurfaceMesh& meshFrom, SurfaceMesh& meshTo,
                                    SurfaceMesh::Vertex_property<Vec2> meshFromMap,
                                    SurfaceMesh::Vertex_property<Vec2> meshToMap);

    static SurfaceMesh SmoothCopy(SurfaceMesh const& mesh, int iterations);

    //rotation from A to B
    static Eigen::Matrix3f ComputeRotationMatrix(Vec3 normA, Vec3 normB);

private:
    static std::map<SurfaceMesh::Vertex, SurfaceMesh::Vertex> MapUVs(SurfaceMesh const& meshFrom, SurfaceMesh const &meshTo,
                                                                     SurfaceMesh::Vertex_property<Vec2> meshFromMap,
                                                                     SurfaceMesh::Vertex_property<Vec2> meshToMap);

    static void ComputeDifferentials(SurfaceMesh const& input, SurfaceMesh::Vertex_property<Vec3>& differentials);
    static void ComputeCotanDifferentials(SurfaceMesh const& mesh, SurfaceMesh::Vertex_property<Vec3>& differentials);
    static void PercentProgress(int size, int iternum);
};
