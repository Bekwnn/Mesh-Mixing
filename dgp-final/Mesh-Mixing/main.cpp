#include <OpenGP/GL/GlfwWindow.h>
#include <OpenGP/SurfaceMesh/SurfaceMesh.h>
#include <OpenGP/SurfaceMesh/GL/SurfaceMeshRenderFlat.h>
#include <OpenGP/SurfaceMesh/GL/SurfaceMeshRenderShaded.h>
#include "internal/ArcballWindow.h"
#include "Mixer.h"
#include "Mapping.h"
using namespace OpenGP;

struct MainWindow : public ArcballWindow{
    SurfaceMesh meshFrom;
    SurfaceMesh meshTo;
    SurfaceMesh meshResult;

    SurfaceMeshRenderFlat rendererFrom = SurfaceMeshRenderFlat(meshFrom);
    SurfaceMeshRenderFlat rendererTo = SurfaceMeshRenderFlat(meshTo);
    SurfaceMeshRenderFlat renderResult = SurfaceMeshRenderFlat(meshResult);

    std::string datadir = "../../dgp-final/data/";
    std::string icoSphere = "icosphere_e.obj";
    std::string hemiSphere = "hemisphere_e.obj";
    std::string thornsPlane = "thornsplane_e.obj";
    std::string spinePlane = "spineplane_e.obj";
    std::string smallSpine = "smallspine_e.obj";
    std::string mountain = "mountaindata_e.obj";
    std::string fractal = "fractal_e.obj";
    std::string perlin = "perlin.obj";
    std::string wool = "wool_e.obj";

    MainWindow() : ArcballWindow(__FILE__,1600,1200){
        bool success = meshFrom.read(datadir + wool);
        if(!success) mFatal() << "File not found: " << wool;
        success = meshTo.read(datadir + icoSphere);
        if(!success) mFatal() << "File not found: " << icoSphere;

        SurfaceMesh::Vertex_property<Vec2> uvCoordFrom = meshFrom.add_vertex_property("uvcoord", Vec2());
        Mapping::PlaneMapping(meshFrom, uvCoordFrom);
        SurfaceMesh::Vertex_property<Vec2> uvCoordTo = meshTo.add_vertex_property("uvcoord", Vec2());
        Mapping::SphereMapping(meshTo, uvCoordTo);

        meshResult = Mixer::ApplyCoating(meshFrom, meshTo, uvCoordFrom, uvCoordTo);

        TranslateMesh(meshTo, Vec3(2.5,0,0));
        TranslateMesh(meshFrom, Vec3(-2.5,0,0));

        // Necessary even for flat shading
        meshFrom.update_face_normals();
        meshTo.update_face_normals();
        meshResult.update_face_normals();

        this->scene.add(rendererFrom);
        this->scene.add(rendererTo);
        this->scene.add(renderResult);
    }

    void key_callback(int key, int scancode, int action, int mods) override{
        ArcballWindow::key_callback(key, scancode, action, mods);
        if(key==GLFW_KEY_SPACE && action==GLFW_RELEASE){
            //do something on space release
        }
    }

    static void TranslateMesh(SurfaceMesh& mesh, Vec3 translate)
    {
        for (auto const& v : mesh.vertices())
        {
            mesh.position(v) += translate;
        }
    }
};

int main(){
    MainWindow window;
    return window.run();
}
