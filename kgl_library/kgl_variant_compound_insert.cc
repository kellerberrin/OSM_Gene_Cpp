//
// Created by kellerberrin on 24/11/17.
//


#include "kgl_variant_compound.h"


namespace kgl = kellerberrin::genome;


std::string kgl::CompoundInsert::mutation(char delimiter, VariantOutputIndex output_index) const
{

  std::stringstream ss;

  ss << "+(" << size() << ")";
  ss << offsetOutput(contigOffset(), output_index);

  return ss.str();

}


bool kgl::CompoundInsert::mutateSequence(SignedOffset_t offset_adjust,
                                         std::shared_ptr<DNA5SequenceLinear> dna_sequence_ptr,
                                         SignedOffset_t& sequence_size_modify) const {

  SignedOffset_t adjusted_offset = offset() + offset_adjust;

  // Check the offset
  if (adjusted_offset < 0 or adjusted_offset >= static_cast<SignedOffset_t>(dna_sequence_ptr->length())) {

    ExecEnv::log().error("mutateSequence(), calculated sequence offset: {} is out of range for sequence size: {}, variant: {}",
                         adjusted_offset, dna_sequence_ptr->length(), output(' ', VariantOutputIndex::START_0_BASED, true));
    return false;
  }

  auto sequence_offset = static_cast<ContigOffset_t>(adjusted_offset);

  auto reference_offset = sequence_offset;

  StringDNA5 seq_string;

  for (auto variant : getMap()) {

    std::shared_ptr<const InsertVariant> insert_ptr = std::dynamic_pointer_cast<const InsertVariant>(variant.second);

    if (not insert_ptr) {

      ExecEnv::log().error("mutateSequence(), compound insert contains unexpected variant: {}",
                           variant.second->output(' ', VariantOutputIndex::START_0_BASED, true));
      return false;

    }

    // Check the reference.
    if (insert_ptr->reference() != dna_sequence_ptr->at(reference_offset)) {

      ExecEnv::log().info("mutateSequence(), Insert reference base: {} does not match sequence base: {} at contig: {} offset: {}",
                          DNA5::convertToChar(insert_ptr->reference()),
                          DNA5::convertToChar(dna_sequence_ptr->at(sequence_offset)),
                          insert_ptr->contig()->contigId(), insert_ptr->offset());

    }

    seq_string.push_back(insert_ptr->mutant());

    ++reference_offset;

  }

  // Mutate the sequence
  DNA5SequenceLinear insert_seq(seq_string);

  if (dna_sequence_ptr->insertSubSequence(sequence_offset, insert_seq)) {

    sequence_size_modify = insert_seq.length();

  } else {

    sequence_size_modify = 0;

  }

  return true;

}
