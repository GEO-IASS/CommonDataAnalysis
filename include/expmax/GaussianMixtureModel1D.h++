/**
 * @file GaussianMixtureModel1D.h++
 *
 * @author Erik Flick <erik.flick [AETT] informatik.uni-hamburg.de>
 *
 *  Created on: Jan 5, 2011
 *
 * @section NOTE
 * This is the class for 1D GMM which can actually be used.
 *
 */

#pragma once

#include "FitUnivariateMulticlassByEM.h++"

#include <boost/function.hpp>

namespace CDA {

/**
 * @class GaussianMixtureModel1D
 *
 * @author flick
 *
 * @brief
 * A GaussianMixtureModel class, with EM fitting routine built in
 */
class MYEXPORT GaussianMixtureModel1D : public FitUnivariateMulticlassByEM {

public:
    /**
     * @brief Re-define datapoint type
     */
    typedef double datapoint_t;
    typedef FitUnivariateMulticlassByEM::data_t data_t;
    typedef FitUnivariateMulticlassByEM::theta_t theta_t;

private:

    using EM<data_t, theta_t>::getThetaObj;

    /**
     * @brief Number of clusters
     */
    using FitUnivariateMulticlassByEM::K;

    /**
     * Get distance to mean
     *
     * @param k class no.
     * @param x
     * @return \f$\left|x-m_k\right|\f$
     */
    inline double squaredDistanceToMean(const unsigned k, const datapoint_t x) const;

protected:

    /**
     * @brief <b>E-step</b>: update hidden attributes, i.e. class membership probabilities
     */
    void update_thetas();

public:

    /**
     * @brief Evaluate the cluster model PDF with class parameters \f$k\f$ at \f$\vec{x}\f$
     *
     * @param[in] k class
     * @param[in] x feature vector
     *
     * @return probability \f$p(x,k\vert\theta)\f$
     */
    inline double evalPDF(const unsigned k, const datapoint_t& x) const;

    /**
     * @brief Call superclass constructor to fill data fields
     */
    GaussianMixtureModel1D(const unsigned K_, const data_t& data, const theta_t& theta);

    /**
     * @brief Estimated parameter getter.
     * Get current estimated mean vector of class k
     *
     * @param[in] k class no.
     */
    double getMean(const unsigned k) const;

    /**
     * @brief Estimated parameter getter
     * Get current estimated sigma of class k
     *
     * @param[in] k class no.
     */
    double getSigma(const unsigned k) const;

    /**
     * @brief Initialize known samples, i.e. your data points
     *
     * @param[in] data_ A pair of iterators.
     * @todo we should check II for compliance.
     */
    template<class II>
    void setData(std::pair<II, II> data_) {
        FitUnivariateMulticlassByEM::setData(data_);
    }

    /**
     * @brief Find discriminant between two classes
     *
     * @param[in] k1 a class
     * @param[in] k2 a class
     *
     * At least this. Is there an easier way?
     *         // @todo double-check
     */
    boost::function<double(const double)>
    getDiscriminantFunction(const int k1, const int k2) const;

    /**
     * @brief Calculate explicit class boundary
     *
     * @param[in] k1 Class 1
     * @param[in] k2 Class 2
     *
     * @return x>value: in class 2
     */
    double findDecisionLimit(const int k1, const int k2) const;

    /**
     * @brief Class name for logging ...
     */
    const std::string className() const;

    /**
     * @brief Each class gets its own line, that's the easiest way
     *
     * @return a "\n"-terminated string.
     */
    const std::string getCSVHeader() const;

};

} // namespace
