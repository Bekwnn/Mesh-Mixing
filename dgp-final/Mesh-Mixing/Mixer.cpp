#include "Mixer.h"
#include "Mapping.h"
using namespace OpenGP;

SurfaceMesh Mixer::ApplyCoating(SurfaceMesh const& meshFrom, SurfaceMesh const& meshTo,
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
    SurfaceMesh resultMesh;

    std::map<SurfaceMesh::Vertex, SurfaceMesh::Vertex> vertexMappingStoU;

    std::cout << "starting map" << std::endl;

    for (auto vertS : meshFrom.vertices())
    {
        Vec2 S = meshFromMap[vertS];
        SurfaceMesh::Vertex closest;
        float bestDist = INFINITY;
        for (auto vertU : meshTo.vertices())
        {
            Vec2 U = meshToMap[vertU];
            float udiff = S[0]-U[0];
            float vdiff = S[1]-U[1];

            float vertDist = fabs(sqrt(udiff*udiff + vdiff*vdiff));
            if (vertDist > bestDist)
            {
                bestDist = vertDist;
                closest = vertU;
            }
        }

        //map(vertS, closest)
        vertexMappingStoU.insert(std::make_pair(vertS, closest));
    }

    std::cout << "finished mapping" << std::endl;

    //RETURN FINAL RESULT
    return SurfaceMesh();
}


void Mixer::ComputeDifferentials(SurfaceMesh mesh, SurfaceMesh::Vertex_property<Vec3>& differentials)
{
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
    Eigen::SparseMatrix<OpenGP::Scalar> L;

    SurfaceMesh::Halfedge betaEdge, alphaEdge;
    SurfaceMesh::Vertex vBeta, vAlpha, v_j;
    Point p_i, p_j, p_b, p_a, d_ib, d_ia, d_aj, d_bj, d_ij;
    Scalar alpha, beta, area, cotanAlpha, cotanBeta, cotanSum;

    SparseMatrix<Scalar> M(n, n);
    SparseMatrix<Scalar> D(n, n);

    std::vector<Triplet> mList;
    std::vector<Triplet> diagonalList;

    mList.reserve(n * n);
    diagonalList.reserve(n * n);

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

    }

    // Compute the final Laplacian and return it.
    M.setFromTriplets(mList.begin(), mList.end());
    D.setFromTriplets(diagonalList.begin(), diagonalList.end());

    L = D * M;
}


