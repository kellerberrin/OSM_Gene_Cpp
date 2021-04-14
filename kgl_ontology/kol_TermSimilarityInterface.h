/*=============================================================================
Copyright (c) 2016 Paul W. Bible
Distributed under the Boost Software License, Version 1.0.
==============================================================================*/
#ifndef KGL_TERM_SIMILARITY_INTERFACE
#define KGL_TERM_SIMILARITY_INTERFACE

#include <string>
#include "kol_GoEnums.h"


namespace kellerberrin::ontology {

//! An interface class for comparing semantic similarity of GO terms
/*! \class TermSimilarityInterface

	This class defines the interface for comparing term-to-term GO similarity.
*/
class TermSimilarityInterface {

public:

  TermSimilarityInterface() = default;

  virtual ~TermSimilarityInterface() = default;

  //! A pure virtual method for calculating term-to-term similarity for GO terms
  /*!
    This pure virtual method requires similarity measure to implement the basic interface
      that returns a similarity value for two go terms.
  */
  [[nodiscard]] virtual double calculateTermSimilarity(const std::string &goTermA, const std::string &goTermB) const = 0;

  //! A pure virtual method for calculating term-to-term similarity for GO terms
  /*!
    This pure virtual method requires similarity measure to implement the basic interface
      that returns a similarity value for two go terms.
      This version of the function must be normalzied. Returning similarity between 0 and 1
  */
  [[nodiscard]] virtual double calculateNormalizedTermSimilarity(const std::string &goTermA, const std::string &goTermB) const = 0;


};

} // namespace

#endif