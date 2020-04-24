//
// Created by kellerberrin on 23/04/18.
//


#include "kgl_variant_db_unphased.h"
#include "kel_patterns.h"


namespace kgl = kellerberrin::genome;


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// An object that holds variants until they can be phased.
// This object hold variants for a contig.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

// Use this to copy the object.
std::shared_ptr<kgl::UnphasedContig> kgl::UnphasedContig::deepCopy() const {

  std::shared_ptr<UnphasedContig> contig_copy(std::make_shared<UnphasedContig>(contigId()));

  for(auto const& [offset, variant_vector] : getMap()) {

    for (auto const& variant_count : variant_vector) {

      if (not contig_copy->addVariant(variant_count)) {

        ExecEnv::log().error("UnphasedContig::deepCopy; Cannot add Variant to Contig Copy : {}, at Offset: {}", contig_copy->contigId(), offset);

      }

    }

  }

  return contig_copy;

}


bool kgl::UnphasedContig::addVariant(std::shared_ptr<const Variant> variant) {

  auto result = contig_offset_map_.find(variant->offset());

  if (result != contig_offset_map_.end()) {
  // Variant offset exists.

      UnphasedVariantCount new_variant(variant);
      result->second.push_back(new_variant);

  } else {
    // add the new offset.
    std::pair<ContigOffset_t, UnphasedVectorVariantCount> new_offset;
    new_offset.first = variant->offset();
    UnphasedVariantCount new_variant(variant);
    new_offset.second.push_back(new_variant);
    auto result = contig_offset_map_.insert(new_offset);

    if (not result.second) {

      ExecEnv::log().error("UnphasedContig::addVariant(); Could not add variant offset: {} to the genome", variant->offset());
      return false;

    }

  }

  return true;

}


size_t kgl::UnphasedContig::variantCount() const {


  size_t variant_count = 0;

  for (auto const& offset_variant_vector : getMap()) {

      variant_count += offset_variant_vector.second.size();

  }

  return variant_count;

}



std::shared_ptr<kgl::UnphasedContig> kgl::UnphasedContig::filterVariants(const kgl::VariantFilter& filter) const {

  std::shared_ptr<kgl::UnphasedContig> filtered_contig_ptr(std::make_shared<kgl::UnphasedContig>(contigId()));

  // Complements the bool returned by filterVariant(filter) because the delete pattern expects bool true for deletion.
  auto predicate = [&](const UnphasedVectorVariantCount::const_iterator& it) { return not (*it)->filterVariant(filter); };

  for (auto offset_vector : getMap()) {

    UnphasedVectorVariantCount copy_offset_vector = offset_vector.second;

    predicateIterableDelete(copy_offset_vector,  predicate);

    if (not copy_offset_vector.empty()) {

      std::pair<ContigOffset_t, UnphasedVectorVariantCount> new_offset;
      new_offset.first = copy_offset_vector.front()->offset();
      new_offset.second = copy_offset_vector;
      auto result = filtered_contig_ptr->contig_offset_map_.insert(new_offset);

      if (not result.second) {

        ExecEnv::log().error("UnphasedContig::filterVariants; Unable to add duplicate offset: {}, contig: {}", new_offset.first, contigId());

      }

    }

  }

  return filtered_contig_ptr;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// An object that holds variants until they can be phased.
// This object hold variants for a genome.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

// Use this to copy the object.
std::shared_ptr<kgl::UnphasedGenome> kgl::UnphasedGenome::deepCopy() const {

  std::shared_ptr<UnphasedGenome> genome_copy(std::make_shared<UnphasedGenome>(genomeId()));

  for (auto const& [contig_id, contig_ptr] :  getMap()) {

    if (not genome_copy->addContig(contig_ptr->deepCopy())) {

      ExecEnv::log().critical("UnphasedGenome::deepCopy(), Genome: {}, Unable to deepcopy Contig: {}", genomeId(), contig_id);

    }

  }

  return genome_copy;

}


bool kgl::UnphasedGenome::addVariant(std::shared_ptr<const Variant> variant) {

  std::shared_ptr<UnphasedContig> contig_ptr;
  if (not getCreateContig(variant->contigId(), contig_ptr)) {

    ExecEnv::log().error("UnphasedGenome::addVariant(), Genome: {} could not get or create Contig: {}", genomeId(), variant->contigId());
    return false;

  }

  if (not contig_ptr->addVariant(variant)) {

    ExecEnv::log().error("UnphasedGenome::addVariant(), Genome: {} could not add variant to Contig: {}", genomeId(), variant->contigId());
    return false;

  }

  return true;

}


bool kgl::UnphasedGenome::getCreateContig(const ContigId_t& contig_id, std::shared_ptr<UnphasedContig>& contig_ptr) {

  auto result = contig_map_.find(contig_id);

  if (result != contig_map_.end()) {

    contig_ptr = result->second;
    return true;

  } else {

    contig_ptr = std::make_shared<UnphasedContig>(contig_id);
    std::pair<ContigId_t, std::shared_ptr<UnphasedContig>> new_contig(contig_id, contig_ptr);
    auto result = contig_map_.insert(new_contig);

    if (not result.second) {

      ExecEnv::log().critical("UnphasedGenome::getCreateContig(), Serious Error, could not add contig: {} to the genome", contig_id);

    }

    return result.second;

  }

}


bool kgl::UnphasedGenome::addContig(std::shared_ptr<UnphasedContig> contig_ptr) {

  std::pair<ContigId_t, std::shared_ptr<UnphasedContig>> add_contig(contig_ptr->contigId(), contig_ptr);
  auto result = contig_map_.insert(add_contig);

  if (not result.second) {

    ExecEnv::log().error("UnphasedGenome::addContig(); could not add contig: {} to the genome", contig_ptr->contigId());

  }

  return result.second;

}



size_t kgl::UnphasedGenome::variantCount() const {


  size_t variant_count = 0;

  for (auto contig : getMap()) {

    variant_count += contig.second->variantCount();

  }

  return variant_count;

}



std::shared_ptr<kgl::UnphasedGenome> kgl::UnphasedGenome::filterVariants(const kgl::VariantFilter& filter) const {

  std::shared_ptr<kgl::UnphasedGenome> filtered_genome_ptr(std::make_shared<kgl::UnphasedGenome>(genomeId()));

  for (const auto& contig_variant : getMap()) {

    std::shared_ptr<kgl::UnphasedContig> filtered_contig = contig_variant.second->filterVariants(filter);
    if (not filtered_genome_ptr->addContig(filtered_contig)) {

      ExecEnv::log().critical("UnphasedGenome::filterVariants(), Genome: {}, Unable to inserted filtered Contig: {}", genomeId(), filtered_contig->contigId());

    }

    ExecEnv::log().vinfo("Contig: {} has: {} filtered variants", contig_variant.first, filtered_contig->variantCount());

  }

  return filtered_genome_ptr;

}

