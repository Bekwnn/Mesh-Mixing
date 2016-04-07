#include <OpenGP/GL/GlfwWindow.h>
#include <OpenGP/SurfaceMesh/SurfaceMesh.h>
#include <OpenGP/SurfaceMesh/GL/SurfaceMeshRenderFlat.h>
using namespace OpenGP;

int main()
{
    SurfaceMesh mesh;

    SurfaceMesh::Vertex v1 = mesh.add_vertex(Vec3(0, 0, 0));
    SurfaceMesh::Vertex v2 = mesh.add_vertex(Vec3(0.6, 0, 0));
    SurfaceMesh::Vertex v3 = mesh.add_vertex(Vec3(0, 0.6, 0));
    mesh.add_triangle(v1, v2, v3);

    mesh.update_face_normals();

    SurfaceMesh meshL;

    SurfaceMesh::Vertex v1L = meshL.add_vertex(Vec3(0, 0, 0));
    SurfaceMesh::Vertex v2L = meshL.add_vertex(Vec3(-0.6, 0, 0));
    SurfaceMesh::Vertex v3L = meshL.add_vertex(Vec3(0, 0.6, 0));
    meshL.add_triangle(v1L, v3L, v2L);

    meshL.update_face_normals();

    SurfaceMesh meshR;

    SurfaceMesh::Vertex v1R = meshR.add_vertex(Vec3(0, 0, 0));
    SurfaceMesh::Vertex v2R = meshR.add_vertex(Vec3(-0.6, 0, 0));
    SurfaceMesh::Vertex v3R = meshR.add_vertex(Vec3(0, -0.6, 0));
    meshR.add_triangle(v1R, v2R, v3R);

    meshR.update_face_normals();

    GlfwWindow window("MeshMixing", 800, 800);
    SurfaceMeshRenderFlat render(mesh);
    SurfaceMeshRenderFlat renderL(meshL);
    SurfaceMeshRenderFlat renderR(meshR);
    window.scene.add(render);
    window.scene.add(renderL);
    window.scene.add(renderR);
    window.run();
}
