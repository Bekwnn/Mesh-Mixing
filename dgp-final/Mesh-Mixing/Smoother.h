#pragma once

#include <OpenGP/SurfaceMesh/SurfaceMesh.h>
#include <Eigen/Sparse>

class Smoother
{
public:
    Smoother(OpenGP::SurfaceMesh& mesh);
    ~Smoother();

    void init();

    void use_cotan_laplacian();
    void use_graph_laplacian();

    void smooth_explicit(OpenGP::Scalar lambda);
    void smooth_implicit(OpenGP::Scalar lambda);

private:
    void solve_linear_least_square(Eigen::SparseMatrix<OpenGP::Scalar>& A, Eigen::MatrixXf& B, Eigen::MatrixXf& X);

    OpenGP::SurfaceMesh& mesh;
    Eigen::SparseMatrix<OpenGP::Scalar> L;

};