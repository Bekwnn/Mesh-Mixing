#include <math.h>
#include "Smoother.h"

using namespace Eigen;
using namespace OpenGP;

Smoother::Smoother(OpenGP::SurfaceMesh& mesh) :
    mesh(mesh)
{ }

Smoother::~Smoother()
{ }

void Smoother::init()
{
    int n = mesh.n_vertices();
    L = SparseMatrix<Scalar>(n, n);
}

void Smoother::use_cotan_laplacian()
{
    unsigned int n = mesh.n_vertices();
    typedef Eigen::Triplet<Scalar> Triplet;

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

void Smoother::use_graph_laplacian()
{
    // Grab the number of vertices in the mesh.
    unsigned int n = mesh.n_vertices();

    typedef Eigen::Triplet<Scalar> Triplet;

    std::vector<Triplet> laplacianList;
    laplacianList.reserve(n * n);

    int degree;
    for (auto const& v_i : mesh.vertices())
    {
        degree = 0;
        for (auto const& edge : mesh.halfedges(v_i))
        {
            auto v_j = mesh.to_vertex(edge);

            // Insert -1 on the corresponding entry in the matrix.
            laplacianList.push_back(Triplet(v_i.idx(), v_j.idx(), -1));

            // Increase the degree of the v_i.
            degree++;
        }

        // Now insert the degree of the v_i into the diagonal.
        laplacianList.push_back(Triplet(v_i.idx(), v_i.idx(), degree));
    }

    L.setFromTriplets(laplacianList.begin(), laplacianList.end());
}

void Smoother::smooth_explicit(OpenGP::Scalar lambda)
{
    // Set up our identity matrix.
    int n = mesh.n_vertices();
    SparseMatrix<Scalar> I(n, n);
    I.setIdentity();

    MatrixXf P_t(3, mesh.n_vertices());
    MatrixXf P_t1(3, mesh.n_vertices());

    // Now create the matrix containing all of our points.
    int i = 0;
    for (auto const& vertex : mesh.vertices())
    {
        Point p = mesh.position(vertex);
        P_t.col(i) = p;
        ++i;
    }

    P_t.transposeInPlace();
    P_t1.transposeInPlace();

    P_t1 = (I - lambda * L) * P_t;

    // Now assign the points back into the mesh.
    i = 0;
    for (auto const& vertex : mesh.vertices())
    {
        mesh.position(vertex) = P_t1.row(i);
        i++;
    }
}

void Smoother::smooth_implicit(OpenGP::Scalar lambda)
{
    int n = mesh.n_vertices();
    SparseMatrix<Scalar> I(n, n);
    I.setIdentity();

    MatrixXf P_t(3, mesh.n_vertices());
    MatrixXf P_t1(3, mesh.n_vertices());

    int i = 0;
    for (auto const& vertex : mesh.vertices())
    {
        Point p = mesh.position(vertex);
        P_t.col(i) = p;
        ++i;
    }

    P_t.transposeInPlace();
    P_t1.transposeInPlace();

    SparseMatrix<Scalar> A = (I - lambda * L);
    solve_linear_least_square(A, P_t, P_t1);
    P_t = P_t1;

    // Now assign the points back into the mesh.
    i = 0;
    for (auto const& vertex : mesh.vertices())
    {
        mesh.position(vertex) = P_t1.row(i);
        ++i;
    }
}

void Smoother::solve_linear_least_square(Eigen::SparseMatrix<OpenGP::Scalar>& A, Eigen::MatrixXf&B, Eigen::MatrixXf& X)
{
    SparseMatrix<Scalar> At = A.transpose();
    SparseMatrix<Scalar> AtA = At * A;

    typedef SimplicialLDLT<SparseMatrix<Scalar>> Solver;
    Solver solver;
    solver.compute(AtA);

    for (int i = 0; i < B.cols(); ++i)
    {
        X.col(i) = solver.solve(At * B.col(i));
    }
}
