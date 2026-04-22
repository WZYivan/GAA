#pragma once

#include <gaa/core/Eigen/pre.hpp>

#include <gaa/core/kw.hpp>

namespace gaa
{
    GAA_EIGEN_MUT_XPR_FN((void), fill_nan, 1, double val = 0)
    {
        m_1 = m_1.array().isNaN().select(val, m_1.array());
    }

    GAA_EIGEN_XPR_FN((Mat), make_identity_like, 1)
    {
        return Mat::Identity(m_1.rows(), m_1.cols());
    }

    GAA_EIGEN_XPR_FN((Mat), ols_design_matrix_AP, 1, kwargs args = {})
    {
        if (args._has_weight_matrix())
        {
            auto const &p = args.weight_matrix();
            return m_1.transpose() * p * m_1;
        }
        else
        {
            return m_1.transpose() * m_1;
        }
    }

    GAA_EIGEN_XPR_FN((Mat), svd_inverse, 1)
    {
        Eigen::JacobiSVD<Xpr_1> svd(m_1, Eigen::ComputeFullU | Eigen::ComputeFullV);
        gaa_assert(svd.info() == Eigen::Success, "SVD execution failed");

        auto singular_values = svd.singularValues();
        double tolerance = singular_values(0) * std::max(m_1.rows(), m_1.cols()) * 1e-12;

        decltype(singular_values) inv_singular_values(singular_values.size());
        for (int i = 0; i < singular_values.size(); ++i)
        {
            inv_singular_values(i) = (singular_values(i) > tolerance) ? 1.0 / singular_values(i) : 0.0;
        }

        return svd.matrixV() *
               inv_singular_values.asDiagonal() *
               svd.matrixU().transpose();
    }

    GAA_EIGEN_XPR_FN((Mat), cholesky_inverse, 1)
    {
        Eigen::LLT<Xpr_1> llt(m_1);
        if (llt.info() == Eigen::Success)
        {
            return llt.solve(make_identity_like(m_1));
        }

        Eigen::LDLT<Xpr_1> ldlt(m_1);
        if (ldlt.info() == Eigen::Success)
        {
            return ldlt.solve(make_identity_like(m_1));
        }

        return svd_inverse(m_1);
    }

    GAA_EIGEN_XPR_FN((Mat), ols_solve_ALP, 2, kwargs args = {})
    {
        auto const &A = m_1;
        auto const &L = m_2;

        if (args._has_weight_matrix())
        {
            auto const &P = args.weight_matrix();

            auto const &AtPAinv = cholesky_inverse(ols_design_matrix(A, P));
            auto const &AtPL = A.transpose() * P * L;
            return AtPAinv * AtPL;
        }
        else
        {
            return A.colPivHouseholderQr().solve(L);
        }
    }

    extern Mat x_rotation(double omega);
    extern Mat y_rotation(double phi);
    extern Mat z_rotation(double kappa);

    GAA_EIGEN_XPR_FN((Mat), translate, 1, double x, double y, double z)
    {
        gaa_assert(is_coordinates3(m_1));

        Mat translated(m_1.rows(), m_1.cols());

        translated.col(0) = m_1.col(0).array() - x;
        translated.col(1) = m_1.col(1).array() - y;
        translated.col(2) = m_1.col(2).array() - z;

        return translated;
    }

    GAA_EIGEN_XPR_FN((Mat), scale, 1, double s)
    {
        gaa_assert(is_coordinates3(m_1));
        return (m_1.array() * s).matrix();
    }

    GAA_EIGEN_XPR_FN((Mat), rotate_CR, 2)
    {
        gaa_assert(is_coordinates3(m_1));
        gaa_assert(is_rotation_matrix(m_2));

        return m_1 * m_2;
    }

    GAA_EIGEN_XPR_FN((double), median_error_VP, 1, int n, int t, kwargs args = {})
    {
        auto const &V = m_1;
        if (args._has_weight_matrix())
        {
            auto const &P = args.weight_matrix();
            return ((V.transpose() * P * V) / (n - t)).cwiseSqrt()(0);
        }
        else
        {
            return ((V.transpose() * V) / (n - t)).cwiseSqrt()(0);
        }
    }

    namespace EigenFmt
    {
        extern Eigen::IOFormat const python, csv;
    }
}