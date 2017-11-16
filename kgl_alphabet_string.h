//
// Created by kellerberrin on 15/11/17.
//

#ifndef KGL_ALPHABET_STRING_H
#define KGL_ALPHABET_STRING_H



#include <string>
#include <algorithm>
#include "kgl_genome_types.h"
#include "kgl_alphabet_base.h"
#include "kgl_alphabet_amino.h"


namespace kellerberrin {   //  organization level namespace
namespace genome {   // project level namespace


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Template class implements string functionality for the Nucleotide and Amino Acid alphabets.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////



template<typename Alphabet>
class AlphabetString {

public:

  explicit AlphabetString() = default;
  explicit AlphabetString(const std::string& alphabet_str) { convertFromCharString(alphabet_str); }
  explicit AlphabetString(std::basic_string<typename Alphabet::Alphabet> base_string)
  : base_string_(std::move(base_string)) {}
  ~AlphabetString() = default;

  // Iterators to access the underlying std::basic_string
  using const_iterator = typename std::basic_string<typename Alphabet::Alphabet>::const_iterator;
  using const_reverse_iterator = typename std::basic_string<typename Alphabet::Alphabet>::const_reverse_iterator;
  using value_type = typename Alphabet::Alphabet;

  const_iterator begin() const { return base_string_.begin(); }
  const_reverse_iterator rbegin() const { return base_string_.rbegin(); }
  void push_back(typename Alphabet::Alphabet nucleotide) { base_string_.push_back(nucleotide); }

  AlphabetString& operator=(const AlphabetString& copy) = default;

  AlphabetString substr(ContigOffset_t offset, ContigSize_t size)
  {

    return AlphabetString(base_string_.substr(offset, size));

  }

  ContigSize_t length() const { return base_string_.length(); }

  void reserve(ContigSize_t string_size) { base_string_.reserve(string_size); }

  typename Alphabet::Alphabet operator[] (ContigOffset_t& offset) const { return base_string_[offset]; }

  void modifyNucleotide(ContigOffset_t& offset,
                        typename Alphabet::Alphabet nucleotide) { base_string_[offset] = nucleotide; }

  std::string str() const { return convertToCharString(); }

private:

  std::basic_string<typename Alphabet::Alphabet> base_string_;

  std::string convertToCharString() const;
  void convertFromCharString(const std::string &alphabet_str);


};


template<typename Alphabet>
std::string AlphabetString<Alphabet>::convertToCharString() const {

  std::string base_string;
  base_string.reserve(length());

  auto convert_base = [](typename Alphabet::Alphabet base) { return static_cast<char>(base); };
  std::transform(base_string_.begin(), base_string_.end(), std::back_inserter(base_string), convert_base);

  return base_string;

}


template<typename Alphabet>
void AlphabetString<Alphabet>::convertFromCharString(const std::string &alphabet_str) {

  base_string_.reserve(alphabet_str.length());
  std::transform(alphabet_str.begin(), alphabet_str.end(), std::back_inserter(base_string_), Alphabet::convertChar);

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The alphabet strings are defined here.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// A string of the standard 5 nucleotide DNA/RNA alphabet A, C, G, T/U, N
using StringDNA5 = AlphabetString<DNA5>;

// A string of the extended DNA5 alphabet to include deletions and insertions.
// Note that X = Delete, E = A insert, F = C insert, I = G insert, J = T insert and K = N insert.
using StringExtendDNA5 = AlphabetString<ExtendDNA5>;

// Defines a string of the Amino Acid alphabet
using StringAminoAcid = AlphabetString<AminoAcid>;


}
}


#endif //KGL_ALPHABET_STRING_H