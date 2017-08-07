#include "meanvariance.hpp"
#include <iostream>

namespace pfopt {

    MeanVariance::MeanVariance(const VectorXd& expectReturn,
                               const MatrixXd& varMatrix,
                               int numOfConstrains,
                               std::list<VectorXi> industryCodesIndexLists,
                               VectorXd industryWeightsConstrains,
                               double riskAversion)
        : expectReturn_(expectReturn), varMatrix_(varMatrix), riskAversion_(riskAversion), feval_(0.),
          numOfConstrains_(numOfConstrains), industryCodesIndexLists_(industryCodesIndexLists),
          industryWeightsConstrains_(industryWeightsConstrains) {
        assert(expectReturn_.size() == varMatrix.rows());
        assert(varMatrix.rows() == varMatrix.cols());
        numOfAssets_ = static_cast<int>(expectReturn_.size());
        xReal_.resize(numOfAssets_, 1);
        grad_f_.resize(numOfAssets_, 1);
        x_.resize(numOfAssets_, 1);
    }

    bool MeanVariance::setBoundedConstraint(const VectorXd& lb, const VectorXd& ub) {
        assert(lb.size() == numOfAssets_);
        assert(ub.size() == numOfAssets_);
        lb_ = lb;
        ub_ = ub;
        return true;
    }

    bool
    MeanVariance::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g, Index& nnz_h_lag, IndexStyleEnum& index_style) {
        n = numOfAssets_;
        m = numOfConstrains_;
        nnz_jac_g = numOfAssets_;

        for (VectorXi codeList : industryCodesIndexLists_) {
            nnz_jac_g += codeList.size();
        }

        //	std::cout << "industryCodesIndexLists_ size: " << industryCodesIndexLists_.size() << std::endl;

        index_style = TNLP::C_STYLE;
        return true;
    }

    bool MeanVariance::get_bounds_info(Index n, Number* x_l, Number* x_u, Index m, Number* g_l, Number* g_u) {
        for (Index i = 0; i < numOfAssets_; ++i) {
            x_l[i] = lb_(i);
            x_u[i] = ub_(i);
        }
        g_l[0] = -9999.; // ������=1����
        g_u[0] = 9999.;  // ������=1����
        // g_l[0] = g_u[0] = 1.0
        // AX=B ʵ����������Ϊ  GL<<AX<<GU   GL=GU=1����ʾΪ AX=1

        for (Index i = 1; i < numOfConstrains_; ++i) {
            g_l[i] = g_u[i] = industryWeightsConstrains_(i - 1);
            //		std::cout << "Constrain " << i << " is " << industryWeightsConstrains_(i-1);
        }
        //	std::cout << "numOfConstrains_ " << numOfConstrains_<<std::endl;
        return true;
    }

    bool MeanVariance::get_starting_point(Index n,
                                          bool init_x,
                                          Number* x,
                                          bool init_z,
                                          Number* z_L,
                                          Number* z_U,
                                          Index m,
                                          bool init_lambda,
                                          Number* lambda) {
        double startWeight = 1. / numOfAssets_;
        for (Index i = 0; i < numOfAssets_; ++i) {
            x[i] = startWeight;
        }
        return true;
    }

    bool MeanVariance::eval_f(Index n, const Number* x, bool new_x, Number& obj_value) {
        for (int i = 0; i < numOfAssets_; ++i)
            xReal_(i) = x[i];
        // risk grad
        VectorXd riskGrad = varMatrix_ * xReal_;
        obj_value = 0.5 * riskAversion_ * xReal_.dot(riskGrad) - expectReturn_.dot(xReal_);
        grad_f_ = riskAversion_ * riskGrad - expectReturn_;
        return true;
    }

    bool MeanVariance::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f) {
        if (new_x) {
            for (int i = 0; i < numOfAssets_; ++i)
                xReal_(i) = x[i];
            // risk grad
            VectorXd riskGrad = varMatrix_ * xReal_;
            grad_f_ = riskAversion_ * riskGrad - expectReturn_;
            for (int i = 0; i < numOfAssets_; ++i) {
                grad_f[i] = grad_f_(i);
            }
        } else
            for (Index i = 0; i < numOfAssets_; ++i)
                grad_f[i] = grad_f_(i);
        return true;
    }

    bool MeanVariance::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g) {
        g[0] = 0.;
        for (Index i = 0; i < numOfAssets_; ++i)
            g[0] += x[i];

        int index = 1;
        for (VectorXi codeList : industryCodesIndexLists_) {
            g[index] = 0.;
            for (int i = 0; i < codeList.size(); i++) {
                g[index] += x[codeList(i)];
                //        std::cout << "VectorXi " << index << " X array index is  " << codeList(i);
            }
            index++;
        }
        //	std::cout << "industryCodesIndexLists_ size: " << industryCodesIndexLists_.size() << std::endl;

        return true;
    }

    bool MeanVariance::eval_jac_g(
        Index n, const Number* x, bool new_x, Index m, Index nele_jac, Index* iRow, Index* jCol, Number* values) {
        if (values == NULL) {
            for (Index i = 0; i < numOfAssets_; ++i) {
                iRow[i] = 0;
                jCol[i] = i;
            }

            Index next_non_null = numOfAssets_;
            Index i = 0;
            for (VectorXi codeList : industryCodesIndexLists_) {
                for (Index j = 0; j != codeList.size(); ++j) {
                    iRow[next_non_null] = i + 1;
                    jCol[next_non_null] = codeList(j);
                    ++next_non_null;
                }
                ++i;
            }
        } else {
            for (Index i = 0; i < numOfAssets_; ++i) {
                values[i] = 1.;
            }

            Index next_non_null = numOfAssets_;
            for (VectorXi codeList : industryCodesIndexLists_) {
                for (Index j = 0; j != codeList.size(); ++j) {
                    values[next_non_null] = 1.;
                    ++next_non_null;
                }
            }
        }
        return true;
    }

    void MeanVariance::finalize_solution(SolverReturn status,
                                         Index n,
                                         const Number* x,
                                         const Number* z_L,
                                         const Number* z_U,
                                         Index m,
                                         const Number* g,
                                         const Number* lambda,
                                         Number obj_value,
                                         const IpoptData* ip_data,
                                         IpoptCalculatedQuantities* ip_cq) {
        for (Index i = 0; i < numOfAssets_; ++i)
            x_[i] = x[i];
        feval_ = obj_value;
    }
}
