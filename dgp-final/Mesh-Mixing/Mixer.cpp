#include <Eigen/Sparse>
#include "Mixer.h"
#include "Mapping.h"
#include "internal/SurfaceMeshVerticesKDTree.h"
#include "Smoother.h"
using namespace OpenGP;

/// Applies the high frequency details (ie texture) of meshFrom to the surface of meshTo
/// using laplacian coordinates and surface editing
SurfaceMesh Mixer::ApplyCoating(SurfaceMesh& meshFrom, SurfaceMesh& meshTo,
                                SurfaceMesh::Vertex_property<Vec2> meshFromMap,
                                SurfaceMesh::Vertex_property<Vec2> meshToMap)
{
    /* TODO:
     *  - use the uv coordinates to map vertices from one mesh to the other
     *  - set the result map to be equal to the meshTo mesh
     *  - capture the high freq details of meshFrom (S) by diffing it with a smoothed version of itself (S*)
     *  - encode the coating based on laplacian coordinates L (of S) and L* (of S*) then encoding of vertex i is L - L*
     *  - meshTo (U). Align S to U using surface normals of S* and U* (in our case U is a sphere and U = U*)
     *  - add L - L* to the laplacian of U, with respect to rotation alignment R for a vertex j where i maps to j
     *  - use the new laplacian to get the new vertex position
     *  - ???
     *  - profit!
     */
    SurfaceMesh resultMesh(meshTo);

    std::map<SurfaceMesh::Vertex, SurfaceMesh::Vertex> vertexMappingStoU
            = MapUVs(meshFrom, meshTo, meshFromMap, meshToMap);

    SurfaceMesh::Vertex_property<Vec3> differentialsFrom = meshFrom.add_vertex_property("differentials", Vec3());
    ComputeDifferentials(meshFrom, differentialsFrom);

    SurfaceMesh smoothFrom = SmoothCopy(meshFrom, 40);

    return resultMesh;
}

/// Takes two meshes and their corresponding UV maps to return a vertex mapping from
/// meshFrom to meshTo. The mapping is not 1:1 nor onto.
std::map<SurfaceMesh::Vertex, SurfaceMesh::Vertex> Mixer::MapUVs(SurfaceMesh const& meshFrom, SurfaceMesh const& meshTo,
                                                                 SurfaceMesh::Vertex_property<Vec2> meshFromMap,
                                                                 SurfaceMesh::Vertex_property<Vec2> meshToMap)
{
    std::cout << "UVMapping begins:" << std::endl;

    std::map<SurfaceMesh::Vertex, SurfaceMesh::Vertex> vertexMappingStoU;

    std::vector<std::pair<SurfaceMesh::Vertex, SurfaceMesh::Vertex>> vertexMapping;

    // Construct 3D mesh representative of meshFrom's uv mapping
    SurfaceMesh flatU(meshTo);
    for (auto vertU : meshTo.vertices())
    {
        Vec2 tempv2 = meshToMap[vertU];
        flatU.position(vertU) = Vec3(tempv2[0], tempv2[1], 0);
    }

    SurfaceMeshVerticesKDTree meshToTree(flatU);

    // Find closest vertex mapping via uv coordinates
    size_t vertexID = 0;
    for (auto vertS : meshFrom.vertices())
    {
        Vec2 S = meshFromMap[vertS];
        vertexMapping.push_back( std::make_pair(vertS, SurfaceMesh::Vertex( meshToTree.closest_vertex( Vec3(S[0], S[1], 0)).idx() ) ) );

        // Progress statement
        vertexID++;
        PercentProgress(meshFrom.n_vertices(), vertexID);
    }

    vertexMappingStoU.insert(begin(vertexMapping), end(vertexMapping));

    std::cout << "UVMapping finished." << std::endl;

    return vertexMappingStoU;
}

/// Computes the laplacian coordinates using cotangent weights
void Mixer::ComputeDifferentials(SurfaceMesh const& mesh, SurfaceMesh::Vertex_property<Vec3>& differentials)
{
    std::cout << "Computing differentials:" << std::endl;

    /*
     * Compute the Laplacian Coordinates of a mesh using uniform weights
    for (auto v_i : input.vertices())
    {
        Vec3 p = input.position(v_i);
        int d = 0;
        Vec3 sum(0.0f);
        for (auto v_j : input.vertices(v_i))
        {
            d++;
            sum += input.position(v_j);
        }

        differentials[v_i] = p - sum * ( 1.0 / (float) d);
    }
     */
    unsigned int n = mesh.n_vertices();
    typedef Eigen::Triplet<Scalar> Triplet;
    Eigen::SparseMatrix<Scalar> L;

    SurfaceMesh::Halfedge betaEdge, alphaEdge;
    SurfaceMesh::Vertex vBeta, vAlpha, v_j;
    Point p_i, p_j, p_b, p_a, d_ib, d_ia, d_aj, d_bj, d_ij;
    Scalar alpha, beta, area, cotanAlpha, cotanBeta, cotanSum;

    Eigen::SparseMatrix<Scalar> M(n, n);
    Eigen::SparseMatrix<Scalar> D(n, n);

    std::vector<Triplet> mList;
    std::vector<Triplet> diagonalList;

    mList.reserve(n * n);
    diagonalList.reserve(n * n);

    size_t vertexID = 0;
    for (auto const& v_i : mesh.vertices())
    {
        cotanSum = 0.0f;
        area = 0.0f;

        for (auto const& edge : mesh.halfedges(v_i))
        {
            // Grab the vertex that the current edge points to.
            v_j = mesh.to_vertex(edge);

            // Now grab the edges that point to the two vertices
            // that define alpha and beta.
            betaEdge = mesh.next_halfedge(edge);
            alphaEdge = mesh.next_halfedge(mesh.opposite_halfedge(edge));

            // Grab the corresponding vertices.
            vBeta = mesh.to_vertex(betaEdge);
            vAlpha = mesh.to_vertex(alphaEdge);

            // Now get the points to compute the angles.
            p_i = mesh.position(v_i);
            p_j = mesh.position(v_j);
            p_b = mesh.position(vBeta);
            p_a = mesh.position(vAlpha);

            // Set the vectors.
            d_ib = (p_b - p_i).normalized();
            d_ia = (p_a - p_i).normalized();
            d_aj = (p_j - p_a).normalized();
            d_bj = (p_j - p_b).normalized();
            d_ij = (p_j - p_i).normalized();

            // Compute the angles.
            beta = std::acos(d_ib.dot(d_bj));
            alpha = std::acos(d_ia.dot(d_aj));

            // Compute their cotangents.
            cotanAlpha = 1.0f / std::tan(alpha);
            cotanBeta = 1.0f / std::tan(beta);

            // Set the non-diagonal entry.
            mList.push_back(Triplet(v_i.idx(), v_j.idx(), cotanAlpha + cotanBeta));
            cotanSum += cotanAlpha + cotanBeta;

            // Compute the area.
            area += (1 / 6.0f) * (d_ij.cross(d_ia)).norm();
        }

        // Store the negative sum of the cotans in the diagonal.
        mList.push_back(Triplet(v_i.idx(), v_i.idx(), -cotanSum));

        // Store one half of the total area on the diagonal matrix.
        diagonalList.push_back(Triplet(v_i.idx(), v_i.idx(), 0.5f * area));

        // Progress statement
        vertexID++;
        PercentProgress(mesh.n_vertices(), vertexID);
    }

    // Compute the final Laplacian and return it.
    M.setFromTriplets(mList.begin(), mList.end());
    D.setFromTriplets(diagonalList.begin(), diagonalList.end());

    L = D * M;

    std::cout << "Differentials finished." << std::endl;
}

SurfaceMesh Mixer::SmoothCopy(SurfaceMesh const& mesh, int iterations)
{
    std::cout << "Creating smooth copy:" << std::endl;

    SurfaceMesh smoothMeshFrom(mesh);
    Smoother smoother = Smoother(smoothMeshFrom);

    smoothMeshFrom.update_face_normals();
    smoothMeshFrom.update_vertex_normals();

    smoother.init();
    smoother.use_graph_laplacian();

    for (int i = 0; i < iterations; i++)
    {
        smoother.smooth_explicit(0.1f);
        PercentProgress(iterations, i+1);
    }

    std::cout << "Smooth copy finished." << std::endl;

    return smoothMeshFrom;
}

void Mixer::PercentProgress(int size, int iternum)
{
    if (iternum % (size/10) == 0)
    {
        std::cout << "progress: " << (iternum/(size/10))*10 << "%" << std::endl;
    }
}

