/*
#pragma once
#include <OpenGP/types.h>
#include <OpenGP/SurfaceMesh/SurfaceMesh.h>

//=============================================================================
namespace OpenGP {
//=============================================================================

class NanoflannAdapter;
class UVKDTree{
    NanoflannAdapter* _adapter = nullptr; ///< internal
    Mat3xN _data;
public:
    UVKDTree(SurfaceMesh::Vertex_property<Vec2> &uvmapping);
    ~UVKDTree();
    /// Find the closest neighbor to "p"
    SurfaceMesh::Vertex closest_vertex(const Vec2& p) const;
};

//=============================================================================
} // namespace OpenGP
//=============================================================================
*/
