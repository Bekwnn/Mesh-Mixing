#include <OpenGP/SurfaceMesh/SurfaceMesh.h>

class Mixer {
public:
    //TODO: add vertex properties mapping S to U or providing uv coordinates
    //using fixed plane and sphere mappings for now
    static OpenGP::SurfaceMesh ApplyCoating(OpenGP::SurfaceMesh const &meshFrom, OpenGP::SurfaceMesh const& meshTo);
};
