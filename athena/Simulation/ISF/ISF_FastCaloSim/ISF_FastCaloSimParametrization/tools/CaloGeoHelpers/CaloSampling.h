/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef CALOGEOHELPERS_CALOSAMPLING_H
#define CALOGEOHELPERS_CALOSAMPLING_H
/**
   @class CaloSampling
   @brief provides Calorimeter Sampling enum
*/

#include <string>


class CaloSampling {
public:
   // The following enum lists the various calorimeter sampling layers.
   // Calorimeter.  Use this for type safety when calling methods here.
   //
   enum CaloSample {
#define CALOSAMPLING(NAME, ISBARREL, ISENDCAP) NAME,
#include "CaloGeoHelpers/CaloSampling.def"
#undef CALOSAMPLING
   };


   /*! \brief Get number of available samplings */
   static unsigned int getNumberOfSamplings();

   /*! \brief Get a unsigned with one bit set  */
   static unsigned int getSamplingPattern(const CaloSample s) {
     return (0x1U << s);
   }

   /*! \brief Get the bit-pattern for barrel samplings */
   static
#if __cplusplus >= 201100
   constexpr
#endif // C++11
   unsigned int barrelPattern();

   /*! \brief Get the bit-pattern for endcap samplings */
   static
#if __cplusplus >= 201100
   constexpr
#endif // C++11
   unsigned int endcapPattern();

   /*! \brief Returns a string (name) for each CaloSampling
    *
    * @param[in] theSample \p CaloSampling::CaloSample enumerator value
    */
   static std::string getSamplingName (CaloSample theSample);


   /*! \brief Returns a string (name) for each CaloSampling
    *
    * @param[in] theSample \p CaloSampling::CaloSample enumerator value
    */
   static std::string getSamplingName (unsigned int theSample);
};


#if __cplusplus >= 201100
constexpr
#else
inline
#endif // C++11
unsigned int CaloSampling::barrelPattern() {
  return (//EM Barrel
#define CALOSAMPLING(NAME, ISBARREL, ISENDCAP) (((unsigned)ISBARREL)<<NAME) |
#include "CaloGeoHelpers/CaloSampling.def"
#undef CALOSAMPLING
	  0 );
}

#if __cplusplus >= 201100
constexpr
#else
inline
#endif // C++11
unsigned int CaloSampling::endcapPattern() {
  return (//EMEC:
#define CALOSAMPLING(NAME, ISBARREL, ISENDCAP) (((unsigned)ISENDCAP)<<NAME) |
#include "CaloGeoHelpers/CaloSampling.def"
#undef CALOSAMPLING
	  0 );
}

#endif  /* CALOSAMPLING_H */
