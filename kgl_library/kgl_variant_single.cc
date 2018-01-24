//
// Created by kellerberrin on 23/12/17.
//


#include "kgl_variant_single.h"
#include "kgl_sequence_offset.h"


namespace kgl = kellerberrin::genome;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  SingleVariant Variant. A virtual class held in compound variants, produces a modified text output.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::string kgl::SingleVariant::suboutput(char delimiter, VariantOutputIndex output_index, bool detail) const
{
  std::stringstream ss;
  ss << genomeOutput(delimiter, output_index);
  ss << quality() << delimiter;
  ss << subname() << delimiter << size() << delimiter;
  ss << submutation(delimiter, output_index);
  if (detail) {

    ss << evidence()->output(delimiter, output_index);

  }
  ss << '\n';

  return ss.str();

}

std::string kgl::SingleVariant::submutation(char delimiter, VariantOutputIndex output_index) const
{

  std::stringstream ss;

  ss << DNA5::convertToChar(reference()) << offsetOutput(contigOffset(), output_index);
  ss << mutantChar() << delimiter;

  return ss.str();

}

