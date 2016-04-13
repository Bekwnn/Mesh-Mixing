#include <OpenGP/GL/GlfwWindow.h>
#include <OpenGP/SurfaceMesh/SurfaceMesh.h>
#include <OpenGP/SurfaceMesh/GL/SurfaceMeshRenderFlat.h>
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

    std::string datadir = "../../dgp-final/data/";
    std::string icoSphere = "icosphere_e.obj";
    std::string thornsPlane = "thornsplane_e.obj";
    std::string spinePlane = "spineplane_e.obj";

    MainWindow() : ArcballWindow(__FILE__,1600,1200){
        bool success = meshFrom.read(datadir + thornsPlane);
        if(!success) mFatal() << "File not found: " << thornsPlane;
        success = meshTo.read(datadir + icoSphere);
        if(!success) mFatal() << "File not found: " << thornsPlane;

        //meshResult = Mixer::ApplyCoating(meshFrom, meshTo);
        SurfaceMesh::Vertex_property<Vec2> uvCoordFrom = meshFrom.add_vertex_property("uvcoord", Vec2());
        Mapping::PlaneMapping(meshFrom, uvCoordFrom);
        SurfaceMesh::Vertex_property<Vec2> uvSphereCoord = meshTo.add_vertex_property("uvcoord", Vec2());
        Mapping::SphereMapping(meshTo, uvSphereCoord);
        
        TranslateMesh(meshTo, Vec3(1,0,0));
        TranslateMesh(meshFrom, Vec3(-1,0,0));

        meshFrom.update_face_normals();
        meshTo.update_face_normals();

        this->scene.add(rendererFrom);
        this->scene.add(rendererTo);
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
