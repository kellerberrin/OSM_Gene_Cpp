//
// Created by kellerberrin on 14/11/17.
//


#include "kgl_sequence_codon.h"


namespace kgl = kellerberrin::genome;



kgl::Codon::Codon(std::shared_ptr<const DNA5SequenceCoding> sequence_ptr, ContigOffset_t codon_index) {

  if (codon_index >= codonLength(sequence_ptr)) {

    ExecEnv::log().error("Invalid codon specified index:{}, for coding sequence length:{} (first codon returned)",
                         codon_index, sequence_ptr->length());
    codon_index = 0;
  }

  codon_index = static_cast<ContigOffset_t>(codon_index * Codon::CODON_SIZE);

  bases_[0] = sequence_ptr->at(codon_index);
  ++codon_index;
  bases_[1] = sequence_ptr->at(codon_index);
  ++codon_index;
  bases_[2] = sequence_ptr->at(codon_index);

}


void kgl::Codon::modifyBase(size_t index, CodingDNA5::Alphabet base)
{

  if (index >= CODON_SIZE) {

    ExecEnv::log().error("Invalid codon base index specified index:{}, must be < 3)", index);
    return;
  }
  bases_[index] = base;

}
