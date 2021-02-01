//
// Created by kellerberrin on 15/1/21.
//

#ifndef KGL_ANALYSIS_MUTATION_GENE_H
#define KGL_ANALYSIS_MUTATION_GENE_H

#include "kgl_genome_genome.h"
#include "kgl_ped_parser.h"
#include "kgl_variant_db_population.h"
#include "kgl_analysis_mutation_gene_clinvar.h"


namespace kellerberrin::genome {   //  organization::project level namespace



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class GeneCharacteristic {

public:

  GeneCharacteristic() = default;
  ~GeneCharacteristic() = default;

  GeneCharacteristic(const GeneCharacteristic&) = default;
  GeneCharacteristic& operator=(const GeneCharacteristic&) = default;


  GenomeId_t genome;
  ContigId_t contig;
  FeatureIdent_t gene_id;
  std::string gene_name;
  std::string description;
  std::string biotype;
  bool valid_protein{false};
  std::string gaf_id;
  ContigOffset_t gene_begin{0};
  ContigOffset_t gene_end{0};
  ContigSize_t gene_span{0};
  ContigSize_t exons{0};
  ContigSize_t nucleotides{0};
  std::string strand;
  size_t sequences{0};
  std::string seq_name;
  size_t attribute_size{0};
  std::string attributes;

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class VepInfo {

public:

  VepInfo() = default;
  ~VepInfo() = default;

  VepInfo(const VepInfo&) = default;
  VepInfo& operator=(const VepInfo&) = default;


  size_t male_lof{0};          // Loss of gene function in the (B) chromosome.
  size_t female_lof{0};        // Los of function in the female_ (A) chromosome.
  size_t hom_lof{0};          // Loss of gene function in both chromosomes.
  size_t male_high_effect{0};
  size_t female_high_effect{0};
  size_t hom_high_effect{0};          // Loss of gene function in both chromosomes.
  size_t male_moderate_effect{0};
  size_t female_moderate_effect{0};
  size_t hom_moderate_effect{0};          // Loss of gene function in both chromosomes.
  size_t male_modifier_effect{0};
  size_t female_modifier_effect{0};
  size_t hom_modifier_effect{0};          // Loss of gene function in both chromosomes.

};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GeneMutation {

public:

  GeneMutation() = default;
  ~GeneMutation() = default;

  GeneMutation(const GeneMutation&) = default;
  GeneMutation& operator=(const GeneMutation&) = default;

  GeneCharacteristic gene_characteristic;

  size_t unique_variants{0};
  size_t span_variant_count{0};
  size_t variant_count{0};
  size_t male_phase{0};  // Variants from the male_ phased (B) chromosome.
  size_t female_phase{0};  // Variants from the female_ phased (A) chromosome.
  size_t male_lof{0};          // Loss of gene function in the (B) chromosome.
  size_t female_lof{0};        // Los of function in the female_ (A) chromosome.
  size_t hom_lof{0};          // Loss of gene function in both chromosomes.
  size_t male_high_effect{0};          // High Variant Impact in the (B) chromosome.
  size_t female_high_effect{0};        // High Variant Impact in the (A) chromosome.
  size_t hom_high_effect{0};          // High Impact in both in both chromosomes.
  size_t genome_count{0};   // Total number of genomes.
  size_t genome_variant{0};  // Number of genomes that contain variants for this gene.
  size_t homozygous{0};
  size_t heterozygous{0};
  double indel{0.0};
  double transition{0.0};
  double transversion{0.0};
  GeneClinvar clinvar;

};




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GenomeMutation {

public:

  GenomeMutation() = default;
  ~GenomeMutation() = default;

  // This analysis is performed first
  bool genomeAnalysis( const std::shared_ptr<const GenomeReference>& genome_reference);

  // Then this analysis.
  bool variantAnalysis( const std::shared_ptr<const PopulationDB>& population_ptr,
                        const std::shared_ptr<const PopulationDB>& unphased_population_ptr,
                        const std::shared_ptr<const PopulationDB>& clinvar_population_ptr,
                        const std::shared_ptr<const GenomePEDData>& ped_data);
  // Output to file.
  bool writeOutput(const std::shared_ptr<const GenomePEDData>& ped_data, const std::string& out_file, char output_delimiter) const;

  void updatePopulations(const std::shared_ptr<const GenomePEDData>& ped_data);

private:

  std::vector<GeneMutation> gene_vector_;

  // Vep fields.
  constexpr static const char* LOF_VEP_FIELD = "LoF";
  constexpr static const char* LOF_HC_VALUE = "HC";
  constexpr static const char* IMPACT_VEP_FIELD = "IMPACT";
  constexpr static const char* IMPACT_MODERATE_VALUE = "MODERATE";
  constexpr static const char* IMPACT_HIGH_VALUE = "HIGH";

  constexpr static const char* CONCAT_TOKEN = "&";


  static void writeHeader(const std::shared_ptr<const GenomePEDData>& ped_data, std::ostream& out_file, char output_delimiter);

  static void writeClinvar( const std::shared_ptr<const GenomePEDData>& ped_data,
                            const GeneClinvar& results,
                            std::ostream& out_file,
                            char output_delimiter);

  static void writeGene(const GeneCharacteristic& gene, std::ostream& out_file, char output_delimiter);
  static void writeGeneHeader(std::ostream& out_file, char output_delimiter);




  static std::shared_ptr<const ContigDB> getGeneSpan( const std::shared_ptr<const ContigDB>& contig_ptr,
                                                      const GeneCharacteristic& gene_char);

  GeneMutation geneSpanAnalysis( const std::shared_ptr<const PopulationDB>& population_ptr,
                                 const std::shared_ptr<const PopulationDB>& unphased_population_ptr,
                                 const std::shared_ptr<const PopulationDB>& clinvar_population_ptr,
                                 const std::shared_ptr<const GenomePEDData>& ped_data,
                                 GeneMutation gene_mutation);

  VepInfo geneSpanVep( const std::shared_ptr<const ContigDB>& span_contig,
                       const std::shared_ptr<const PopulationDB>& unphased_population_ptr);

  size_t VepCount( const std::shared_ptr<const ContigDB>& vep_contig,
                   const std::string& vep_field_ident,
                   const std::string& vep_field_value);

  void processClinvar(const GenomeId_t& genome_id,
                      const std::shared_ptr<const ContigDB>& gene_variants,
                      const std::shared_ptr<const ContigDB>& clinvar_contig,
                      const std::shared_ptr<const GenomePEDData>& ped_data,
                      GeneClinvar& results);


};




} // namespace




#endif //KGL_KGL_ANALYSIS_MUTATION_GENE_H