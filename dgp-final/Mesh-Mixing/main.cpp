#include <OpenGP/GL/GlfwWindow.h>
#include <OpenGP/SurfaceMesh/SurfaceMesh.h>
#include <OpenGP/SurfaceMesh/GL/SurfaceMeshRenderFlat.h>
#include "ArcballWindow.h"
using namespace OpenGP;

struct MainWindow : public ArcballWindow{
    SurfaceMesh mesh;
    SurfaceMeshRenderFlat renderer = SurfaceMeshRenderFlat(mesh);

    std::string datadir = "../../dgp-final/data/";
    std::string icoSphere = "icosphere_e.obj";
    std::string thornsPlane = "thornsplane_e.obj";
    std::string spinePlane = "spineplane_e.obj";

    MainWindow() : ArcballWindow(__FILE__,1600,1200){
        bool success = mesh.read(datadir + thornsPlane);
        if(!success) mFatal() << "File not found: " << thornsPlane;
        mesh.update_face_normals();
        this->scene.add(renderer);
    }

    void key_callback(int key, int scancode, int action, int mods) override{
        ArcballWindow::key_callback(key, scancode, action, mods);
        if(key==GLFW_KEY_SPACE && action==GLFW_RELEASE){
            //do something on space release
        }
    }
};

int main(){
    MainWindow window;
    return window.run();
}
