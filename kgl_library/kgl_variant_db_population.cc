//
// Created by kellerberrin on 23/04/18.
//


#include <memory>
#include <fstream>
#include "kgl_patterns.h"
#include "kgl_variant_compound.h"
#include "kgl_variant_db.h"
#include "kgl_sequence_offset.h"

namespace kgl = kellerberrin::genome;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple container to hold genome variants for populations
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool kgl::PhasedPopulation::getGenomeVariant(const GenomeId_t& genome_id,
                                             std::shared_ptr<const GenomeVariant>& genome_variant) const {

  auto result = population_variant_map_.find(genome_id);

  if (result != population_variant_map_.end()) {

    genome_variant = result->second;
    return true;

  } else {

    genome_variant = nullptr;
    return false;

  }

}


bool kgl::PhasedPopulation::addGenomeVariant(std::shared_ptr<const GenomeVariant> genome_variant) {

  auto result = population_variant_map_.insert(std::pair<GenomeId_t, std::shared_ptr<const GenomeVariant>>(genome_variant->genomeId(), genome_variant));

  return result.second;

}


size_t kgl::PhasedPopulation::variantCount() const {

  size_t variant_count = 0;
  for (auto genome : getMap()) {

    variant_count += genome.second->variantCount();

  }

  return variant_count;

}


std::shared_ptr<kgl::PhasedPopulation> kgl::PhasedPopulation::filterVariants(const kgl::VariantFilter& filter) const {

  std::shared_ptr<kgl::PhasedPopulation> filtered_population_ptr(std::make_shared<kgl::PhasedPopulation>(populationId()));

  for (const auto& genome_variant : population_variant_map_) {

    std::shared_ptr<kgl::GenomeVariant> filtered_genome_ptr = genome_variant.second->filterVariants(filter);
    filtered_population_ptr->addGenomeVariant(filtered_genome_ptr);
    ExecEnv::log().vinfo("Genome: {} has: {} filtered variants", genome_variant.first, filtered_genome_ptr->variantCount());

  }

  return filtered_population_ptr;

}

