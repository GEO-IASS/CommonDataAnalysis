/**
 * @file GaussianMixtureModelNDParams.c++
 * @version 0.141
 * @author Erik Flick <erik.flick [AETT] informatik.uni-hamburg.de>
 *
 *  Created on: Jan 20, 2011
 *
 */

#include "expmax/GaussianMixtureModelNDParams.h++"
#include "math/matrix_functions.h++"
#include <sstream>

using namespace CDA;

const GaussianMixtureModelNDParams::sym_mtx_t
GaussianMixtureModelNDParams::getSigmaMatrix(const unsigned k) const {
    return thetas[k].get<2>();
}

unsigned GaussianMixtureModelNDParams::getD() const { return D; }
unsigned GaussianMixtureModelNDParams::getK() const { return K; }

GaussianMixtureModelNDParams::GaussianMixtureModelNDParams(const unsigned K_, const unsigned D_)
  : K(K_), D(D_) {
    // It shall never be in an undefined state!
  for (unsigned k=0; k<getK(); ++k) {
      thetas.push_back(pdfparams_t(1/(double)k,
                       boost::numeric::ublas::zero_vector<double>(D),
                       boost::numeric::ublas::identity_matrix<double>(D)));
  }
}

GaussianMixtureModelNDParams::GaussianMixtureModelNDParams(const GaussianMixtureModelNDParams& other)
  : K(other.K), D(other.D) {
    std::copy(other.thetas.begin(), other.thetas.end(), std::back_inserter(thetas));
}

double GaussianMixtureModelNDParams::getSigmaDet(const unsigned k) const {
    return det(getSigmaMatrix(k));
}

const boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::upper>
GaussianMixtureModelNDParams::getInvSigma(const unsigned k) const {

    // @todo improve

    namespace ublas = boost::numeric::ublas;

    ublas::matrix<double> inverse(getD(), getD());
    typedef ublas::permutation_matrix<std::size_t> permatrix;

    // create a working copy of the input, as a matrix (can't be symmetric_matrix, na?)
    ublas::matrix<double> A = getSigmaMatrix(k);

    // create a permutation matrix for the LU-factorization
    permatrix pm(A.size1());

    // perform LU-factorization with pivoting
    lu_factorize(A,pm);

    // create identity matrix of "inverse"
    inverse.assign(ublas::identity_matrix<double>(A.size1()));

    // backsubstitute to get the inverse

    try {
        try {
            lu_substitute(A, pm, inverse);
        } catch ( boost::numeric::ublas::internal_logic& e ) {
            std::cerr << e.what() << std::endl;
        }
    } catch ( std::logic_error& e ) {
        std::cerr << e.what() << "\n";
    }

    // Zufrieden?

    sym_mtx_t result;
    try {
        result = boost::numeric::ublas::triangular_adaptor<boost::numeric::ublas::matrix<double>, boost::numeric::ublas::upper>(inverse);
        // Is there a problem? Just give ZERO
    } catch ( boost::numeric::ublas::external_logic& e ) {
        std::cerr << e.what() << std::endl;
        // result = boost::numeric::ublas::identity_matrix<double>(getD());
        result = boost::numeric::ublas::zero_matrix<double>(getD(), getD());
    }

    return result;
}

void GaussianMixtureModelNDParams::updateCached() {
    m_cached_invsigmas.clear();
    m_cached_sigmadet.clear();
    for (unsigned k=0; k<getK(); ++k) {
        m_cached_invsigmas.push_back(getInvSigma(k));
        m_cached_sigmadet.push_back(getSigmaDet(k));
        // Now only once per turn
#ifdef VERBOSE
        std::cerr << "SIGMA " << getSigmaMatrix(k) << std::endl;
        std::cerr << "INV SIGMA " << getInvSigma(k) << std::endl;
        std::cerr << "DET(SIGMA)" << getSigmaDet(k) << std::endl;
#endif
    }
}

// const boost::numeric::ublas::vector_range<const fvector_t>
const fvector_t&
GaussianMixtureModelNDParams::getMean(const unsigned k) const {
    using namespace boost::numeric::ublas;
    // return project(m_theta.getThetas()[k], range(1, 1+getD()));
    return thetas[k].get<1>();
}

const GaussianMixtureModelNDParams::sym_mtx_t&
GaussianMixtureModelNDParams::getCachedInvSigma(const unsigned k) const {
    return m_cached_invsigmas[k];
}

double
GaussianMixtureModelNDParams::getCachedSigmaDet(const unsigned k) const {
    return m_cached_sigmadet[k];
}


const std::string GaussianMixtureModelNDParams::paramName(const unsigned p) const {
    std::stringstream out;
    if (p < getD()) {
        out << "m_" << p;
    } else {
        out << "s_" << i(p) << "_" << j(p);
    }
    return out.str();
}

