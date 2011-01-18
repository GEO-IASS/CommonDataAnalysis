/**
 * @file GaussianMixtureModelNDCommon.h++
 *
 * @author Erik Flick <erik.flick [AETT] informatik.uni-hamburg.de>
 *
 *  Created on: Jan 5, 2011
 *
 */

#pragma once

#include "EMData.h++"
#include "EMGenericMixtureModelCore.h++"
//#include "EMCustomOptimization.h++"

#include <boost/numeric/ublas/symmetric.hpp>

namespace CDA {

/**
 * @class GaussianMixtureModelNDCommon
 *
 * @brief A heteroscedastic N-dimensional Gaussian Mixture Model
 * minus the parameter optimization part, which is left purevirtual
 * thus a replacement for the EMData<fvector_t> class.
 */
class GaussianMixtureModelNDCommon : public EMData<fvector_t>, public EMGenericMixtureModelCore { // hmk

public:
    typedef fvector_t datapoint_t;

protected:
    /**
     * @brief class beliefs
     */
    using EMGenericMixtureModelCore::classif;
    /**
     * @brief get number of classes
     */
    using EMGenericMixtureModelCore::P;
    using EMGenericMixtureModelCore::K;
//
//    /**
//     * @brief Anyway: let's introduce a new version here
//     *
//     * @return this
//     */
//    virtual EMData<datapoint_t>& getDataObj();

    // using EMData<datapoint_t>::getDataObj; // rather than m_data and rather than EM::'s
    using FitMultivariateMulticlassByEM::getDataObj;

    typedef boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::upper> sym_mtx_t;
    std::vector<sym_mtx_t> m_cached_invsigmas;

public: // why shouldn't they? parameters are set from the outside now
    /**
     * @brief Get param index - D of covariance parameter sigma(i,j) ;-)
     * i.e. does not take into account the other parameters, so the
     * offset still has to be added. clear?
     *
     * @section NOTA BENE
     * <b> \f$i <= j\f$ always! </b>
     * So the upper triangular part of the matrix is stored.
     */
    inline unsigned a(const unsigned i, const unsigned j) const {
        if (i>j) { return a(j,i); } // stupid me
        return i*getD() + j - (i*(i+1))/2;
    }

    /**
     * @brief Get i index of covariance parameter ;-) VERT
     * W/ OFFSET
     */
    inline unsigned i(const unsigned ain, const unsigned iter = 0, bool remove_offset=true) const {
        const unsigned D = getD();
        unsigned a = remove_offset ? ain - D - 1 : ain;
        assert(iter<D);
        if (a>=D-iter) { return i(a-D+iter, iter+1, false); }
        else {
            return a+iter;
        }
    }

    /**
     * @brief Get j index of covariance parameter ;-) HORZ
     * W/ OFFSET
     */
    inline unsigned j(const unsigned ain, const unsigned iter = 0, bool remove_offset=true) const {
        const unsigned D = getD();
        unsigned a = remove_offset ? ain - D - 1 : ain;
        assert(iter<D);
        if (a>=D-iter) { return j(a-D+iter, iter+1, false); }
        else {
            return iter;
        }
    }

    using EMGenericMixtureModelCore::getK;
    using EMGenericMixtureModelCore::getP;
    using EMGenericMixtureModelCore::getD;

public:

    /**
     * @brief Constructor. Calls several superclass ones
     *
     * @param[in] K_
     * @param[in] D_
     *
     * @section Parameter space dimensionality
     * P = D + D(D+1)/2 (mean + covariances)
     *
     * @section WARNING
     * Be careful with the parameter order!
     *
     */
    GaussianMixtureModelNDCommon(const unsigned K_, const unsigned D_);

    /**
     * @brief Construct names of parameters, e.g. for output
     */
    virtual const std::string paramName(const unsigned p) const;

    /**
     * @brief Evaluate model PDF of cluster k
     *
     * @param k class no.
     * @param x feature vector \f$\vec{x}\f$
     *
     * @return \f$p(x,k\vert\theta) = \frac{1}{(2\pi)^{\frac{D}{2}}\left|\Sigma_k\right|}e^{\frac{-1}{2}(\vec{x}-\vec{\mu_k})^T\Sigma_k^{-1}(\vec{x}-\vec{\mu_k})}\f$
     *
     */
    double evalPDF(const unsigned k, const datapoint_t& x) const;

    /**
     * @brief Extract the covariance matrix of Gaussian no. k
     *
     * @param[in] k class no.
     *
     * @return a ublas matrix
     */
    const boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::upper> getSigmaMatrix(const unsigned k) const;

    /**
     * @brief
     *
     * @return det(S)
     */
    double getSigmaDet(const unsigned k) const;

    /**
     * @brief Get inverse of Sigma of Gaussian no. k
     */
    const boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::upper> getInvSigma(const unsigned k) const;

    /**
     * @brief Get Mean vector of Gaussian no. k
     */
    const boost::numeric::ublas::vector_range<const fvector_t> getMean(const unsigned k) const;

    size_t getD() const;

};


} // namespace
