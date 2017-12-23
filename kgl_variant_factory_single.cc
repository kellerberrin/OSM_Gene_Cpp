//
// Created by kellerberrin on 23/12/17.
//


#include "kgl_variant_single.h"
#include "kgl_variant_factory.h"


namespace kgl = kellerberrin::genome;


// Generate SNP variants.
std::shared_ptr<const kgl::GenomeVariant>
kgl::SingleFactory::createSingleVariants(const std::string &genome_name,
                                         const std::shared_ptr<const ContigCountData> &count_data,
                                         const std::shared_ptr<const GenomeDatabase> &genome_db,
                                         NucleotideReadCount_t minimum_read_count,
                                         double minimum_proportion,
                                         Phred_t read_quality) const {

  std::shared_ptr<GenomeVariant> genome_single_variants = kgl::GenomeVariant::emptyGenomeVariant(genome_name, genome_db);
  size_t snp_count = 0;

  for (auto& contig_block : count_data->getMap()) {   // For each contig block.

    // Get the sequence.
    std::shared_ptr<ContigFeatures> contig_ptr;
    if (not genome_db->getContigSequence(contig_block.first, contig_ptr)) {

      ExecEnv::log().error("Contig: {} not found in SingleFactory.create()", contig_block.first);
      continue;

    } else {

      const DNA5SequenceContig& contig_sequence = contig_ptr->sequence();

      const auto &nucleotide_array = contig_block.second->getNucleotideArray();
      for (ContigOffset_t contig_offset = 0; contig_offset < nucleotide_array.contigSize(); ++contig_offset) {

        const NucleotideReadCount_t* nucleotide_count_ptr = nucleotide_array.readCount(contig_offset);

        DNA5::Alphabet reference_nucleotide = contig_sequence[contig_offset];
        ContigOffset_t  reference_column = DNA5::nucleotideToColumn(reference_nucleotide);
        NucleotideReadCount_t read_count = 0;
        for(ContigOffset_t idx = 0; idx <  ExtendDNA5::NUCLEOTIDE_COLUMNS; ++idx) {

          read_count += nucleotide_count_ptr[idx];

        }
        for(ContigOffset_t idx = 0; idx <  ExtendDNA5::NUCLEOTIDE_COLUMNS; ++idx) {

          double proportion = static_cast<double>(nucleotide_count_ptr[idx]) / static_cast<double>(read_count);

          if (idx != reference_column
              and nucleotide_count_ptr[idx] > 0
              and read_count >= minimum_read_count
              and proportion >= minimum_proportion) {

            ExtendDNA5::Alphabet mutant_nucleotide = ExtendDNA5::offsetToNucleotide(idx);

            std::shared_ptr<const VariantEvidence>
            evidence_ptr(std::make_shared<const ReadCountEvidence>(read_count,
                                                                   nucleotide_count_ptr[idx],
                                                                   nucleotide_count_ptr,
                                                                   ExtendDNA5::NUCLEOTIDE_COLUMNS));

            Phred_t quality = evidence_ptr->calculateQuality();

            if (ExtendDNA5::isBaseCode(mutant_nucleotide)) {

              SNPVariant snp_variant(genome_name,
                                     contig_ptr,
                                     contig_offset,
                                     quality,
                                     evidence_ptr,
                                     reference_nucleotide,
                                     mutant_nucleotide);

              addSingleVariant(genome_single_variants, snp_variant); // Annotate with genome information

            } else if (ExtendDNA5::isDeletion(mutant_nucleotide)) {

              DeleteVariant delete_variant(genome_name,
                                           contig_ptr,
                                           contig_offset,
                                           quality,
                                           evidence_ptr,
                                           reference_nucleotide,
                                           mutant_nucleotide);

              addSingleVariant(genome_single_variants, delete_variant); // Annotate with genome information

            } else if (ExtendDNA5::isInsertion(mutant_nucleotide)) {

              InsertVariant insert_variant(genome_name,
                                           contig_ptr,
                                           contig_offset,
                                           quality,
                                           evidence_ptr,
                                           reference_nucleotide,
                                           mutant_nucleotide);

              addSingleVariant(genome_single_variants, insert_variant); // Annotate with genome information

            } else {

              ExecEnv::log().error("ExtendDNA5 Unknown letter type: {}", ExtendDNA5::convertToChar(mutant_nucleotide));

            }

            ++snp_count;

          }

        }

      }  // for all sequence elements

      ExecEnv::log().info("Contig: {} has: {} raw SNPs", contig_ptr->contigId(), snp_count);
      snp_count = 0;

    } // found contig.

  }  // for all contigs.

  return genome_single_variants;

}


// This function will insert multiple variants for each CDS sequence within each gene.
void kgl::SingleFactory::addSingleVariant(std::shared_ptr<GenomeVariant> genome_single_variants,
                                          const Variant &variant) const {

  // Annotate the variant with genome information.
  GeneVector gene_vector;
  ContigOffset_t variant_offset = variant.contigOffset();
  if (variant.contig()->findGenes(variant_offset, gene_vector)) {

    for (const auto& gene_ptr : gene_vector) {

      std::shared_ptr<const CodingSequenceArray> sequence_array = kgl::GeneFeature::getCodingSequences(gene_ptr);
      if (sequence_array->empty()) {

        // create a variant copy and annotate with a gene.
        std::shared_ptr<Variant> intron_single_ptr = variant.clone();
        intron_single_ptr->defineIntron(gene_ptr); // intron
        genome_single_variants->addVariant(intron_single_ptr);

      } else {

        for (const auto& sequence : sequence_array->getMap()) {

          if (sequence.second->isWithinCoding(variant_offset)) {

            // create a variant copy and annotate with a coding sequence.
            std::shared_ptr<Variant> coding_single_ptr = variant.clone();
            coding_single_ptr->defineCoding(sequence.second); // coding
            genome_single_variants->addVariant(coding_single_ptr);

          } else {  // an intron for this sequence

            // create a variant copy and annotate with a gene.
            std::shared_ptr<Variant> intron_single_ptr = variant.clone();
            intron_single_ptr->defineIntron(gene_ptr); // intron
            genome_single_variants->addVariant(intron_single_ptr);

          } // if valid sequence for offset

        } // for all sequences within a gene

      } // if gene has a valid sequence.

    } // for all genes.

  } else {

    // create a variant copy and tag as non-coding.
    std::shared_ptr<Variant> noncoding_single_ptr  = variant.clone();
    noncoding_single_ptr->defineNonCoding(); // non coding
    genome_single_variants->addVariant(noncoding_single_ptr);

  }

}

