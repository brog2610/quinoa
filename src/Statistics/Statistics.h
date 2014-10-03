//******************************************************************************
/*!
  \file      src/Statistics/Statistics.h
  \author    J. Bakosi
  \date      Tue 30 Sep 2014 01:26:37 PM MDT
  \copyright 2005-2014, Jozsef Bakosi.
  \brief     Statistics
  \details   Computing ordinary and central moments
*/
//******************************************************************************
#ifndef Statistics_h
#define Statistics_h

#include <Types.h>
#include <Quinoa/InputDeck/InputDeck.h>
#include <ParticleProperties.h>
#include <UniPDF.h>
#include <BiPDF.h>
#include <TriPDF.h>

namespace quinoa {

extern ctr::InputDeck g_inputdeck;

//! Statistics estimator
class Statistics {

  public:
    //! Constructor
    explicit Statistics( const ParProps& particles );

    //! Accumulate (i.e., only do the sum for) ordinary moments
    void accumulateOrd();

    //! Accumulate (i.e., only do the sum for) central moments
    void accumulateCen( const std::vector< tk::real >& ord );

    //! Accumulate (i.e., only do the sum for) PDFs
    void accumulatePDF();

    //! Ordinary moments accessor
    const std::vector< tk::real >& ord() const noexcept { return m_ordinary; }

    //! Central moments accessor
    const std::vector< tk::real >& ctr() const noexcept { return m_central; }

    //! Univariate PDF accessor
    const std::vector< UniPDF >& updf() const noexcept { return m_updf; }

    //! Bivariate PDF accessor
    const std::vector< BiPDF >& bpdf() const noexcept { return m_bpdf; }

    //! Trivariate PDF accessor
    const std::vector< TriPDF >& tpdf() const noexcept { return m_tpdf; }

  private:
    // Get offset type out of InputDeck for computing map< depvar, offset >
    using ncomps = ctr::InputDeck::nT< tag::component >;

    // Map associating a dependend variable to its offset in the particle data.
    // We use a case-insensitive character comparison functor for the offset
    // map, since the keys (dependent variables) in the offset map are only used
    // to indicate the equation's dependent variable, however, queries (using
    // find) are fired up for both ordinary and central moments (which are upper
    // and lower case) for which the offset (for the same depvar) should be the
    // same.
    using OffsetMap =
      std::map< char, ncomps::ncomp, ctr::CaseInsensitiveCharLess >;

    //! Function object for creating a run-time std::map< depvar, offset >
    struct depvar {
      OffsetMap& m_map;
      explicit depvar( OffsetMap& map ) : m_map(map) {}
      template< typename U > void operator()( U ) {
        // Loop through the vector of dependent variables for a differential
        // equation type given by type U and insert an entry to the offset map
        // for each equation, i.e., handle all potentially multiple equations of
        // the same type.
        ncomps::ncomp c = 0;
        for (const auto& v : g_inputdeck.get< tag::param, U, tag::depvar >())
          m_map[ v ] =
            g_inputdeck.get< tag::component >().template offset< U >( c++ );
      }
    };

    //! Setup ordinary moments
    void setupOrdinary( const OffsetMap& offset );

    //! Setup central moments
    void setupCentral( const OffsetMap& offset );

    //! Setup PDFs
    void setupPDF( const OffsetMap& offset );

    //! Return mean for fluctuation
    int mean(const ctr::Term& term) const;

    const ParProps& m_particles;              //!< Particle properties

    //! Instantaneous variable pointers for computing ordinary moments
    std::vector< std::vector< const tk::real* > > m_instOrd;
    std::vector< tk::real > m_ordinary;        //!< Ordinary moments
    std::vector< ctr::FieldVar > m_ordFieldVar;//!< Ordinary moment field names
    int m_nord;                                //!< Number of ordinary moments

    //! Instantaneous variable pointers for computing central moments
    std::vector< std::vector< const tk::real* > > m_instCen;
    std::vector< tk::real > m_central;         //!< Central moments
    //! Ordinary moments about which to compute central moments
    std::vector< std::vector< const tk::real* > > m_center;
    int m_ncen;                                //!< Number of central moments

    //! Instantaneous variable pointers for computing univariate PDFs
    std::vector< std::vector< const tk::real* > > m_instUniPDF;
    std::vector< UniPDF > m_updf;              //!< Univariate PDFs
    //! Instantaneous variable pointers for computing bivariate PDFs
    std::vector< std::vector< const tk::real* > > m_instBiPDF;
    std::vector< BiPDF > m_bpdf;               //!< Bivariate PDFs
    //! Instantaneous variable pointers for computing trivariate PDFs
    std::vector< std::vector< const tk::real* > > m_instTriPDF;
    std::vector< TriPDF > m_tpdf;              //!< Trivariate PDFs
};

} // quinoa::

#endif // Statistics_h

