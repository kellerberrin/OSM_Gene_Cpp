//
// Created by kellerberrin on 20/4/20.
//

#include "kgl_variant_factory_vcf_parse_info.h"
#include "kgl_variant_factory_grch_impl.h"
#include "kgl_variant_vcf.h"

#include <string>

namespace kgl = kellerberrin::genome;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

void kgl::GrchVCFImpl::processVCFHeader(const VcfHeaderInfo& header_info) {

  // Investigate header.
  VCFContigMap vcf_contig_map;
  VCFInfoRecordMap vcf_info_map;
  if (not VCFParseHeader::parseVcfHeader( header_info, vcf_contig_map, vcf_info_map)) {

    ExecEnv::log().error("GrchVCFImpl::processVCFHeader, Problem parsing header information in VCF file. No variants processed.");

  }

  // Store all the available VCF info fields.
  evidence_factory_.availableInfoFields(vcf_info_map);

  if (VCFParseHeader::checkVCFReferenceContigs(vcf_contig_map, genome_db_ptr_)) {

    ExecEnv::log().info("VCF File and Reference Genome Contig Size and Name All Match");

  } else {

    ExecEnv::log().info("VCF File and Reference Genome Contig Size and Name Mis-Match.");

  }

}


void kgl::GrchVCFImpl::readParseVCFImpl() {



  // multi-threaded
  readVCFFile();
  // single threaded

  for (auto const& [contig_id, count] : contig_count_) {

    ExecEnv::log().info("GrchVCFImpl; Contig id: {}, length: {}, variant count :{}", contig_id, count.first, count.second);

  }

}


void kgl::GrchVCFImpl::ProcessVCFRecord(size_t vcf_record_count, const VcfRecord& vcf_record) {

  // Parse the info fields into a map..
  // For performance reasons the info field is std::moved - don't reference again.
  auto mutable_info = const_cast<std::string&>(vcf_record.info);
  std::optional<std::shared_ptr<InfoDataBlock>> info_evidence_opt = evidence_factory_.createVariantEvidence(std::move(mutable_info));

  // Convert VCF contig to genome contig.
  std::string contig = contig_alias_map_.lookupAlias(vcf_record.contig_id);

  // Check for multiple alt sequences
  size_t position = vcf_record.alt.find_first_of(MULIPLE_ALT_SEPARATOR_);  // Check for ',' separators
  // The alt field can be blank (deletion).
  if (position == std::string::npos or vcf_record.alt.empty()) {

    VariantEvidence evidence(vcf_record_count, info_evidence_opt);
    // Add the variant.
    if (not createAddVariant( vcf_genome_ptr_->genomeId(),
                              contig,
                              vcf_record.offset,
                              vcf_record.ref,
                              vcf_record.alt,
                              evidence)) {

      ExecEnv::log().error("GrchVCFImpl::ProcessVCFRecord, Problem parsing vcf_record.");

    }

    ++variant_count_;

  } else {

    std::vector<std::string> alt_vector = Utility::tokenizer(vcf_record.alt, alt_separator_);

    if (alt_vector.empty()) {

      ExecEnv::log().error("GrchVCFImpl::ProcessVCFRecord, Zero sized alt vector, alt: {}", vcf_record.alt);

    }

    for (auto const& alt : alt_vector) {

      VariantEvidence evidence(vcf_record_count, info_evidence_opt);
      // Add the variant.
      if (not createAddVariant( vcf_genome_ptr_->genomeId(),
                                contig,
                                vcf_record.offset,
                                vcf_record.ref,
                                alt,
                                evidence)) {

        ExecEnv::log().error("GrchVCFImpl::ProcessVCFRecord, Parsing GRCh VCF, Problem parsing vcf_record");

      }

      ++variant_count_;

    }

  }

  if (vcf_record_count % VARIANT_REPORT_INTERVAL_ == 0) {

    ExecEnv::log().info("Processed :{} records, total variants: {}", vcf_record_count, variant_count_);
    ExecEnv::log().info("Contig: {}, offset: {}", contig, vcf_record.offset);

  }

}


bool kgl::GrchVCFImpl::createAddVariant(const GenomeId_t& genome_name,
                                        const ContigId_t& contig_id,
                                        ContigOffset_t contig_offset,
                                        const std::string& reference_text,
                                        const std::string& alternate_text,
                                        const VariantEvidence& evidence)  {

  StringDNA5 reference_str(reference_text);
  StringDNA5 alternate_str(alternate_text);

  std::shared_ptr<const Variant> variant_ptr(std::make_shared<VCFVariant>(genome_name,
                                                                          contig_id,
                                                                          VariantSequence::UNPHASED,
                                                                          contig_offset,
                                                                          evidence,
                                                                          std::move(reference_str),
                                                                          std::move(alternate_str)));

  return addThreadSafeVariant(variant_ptr);

}


bool kgl::GrchVCFImpl::addThreadSafeVariant(std::shared_ptr<const Variant>& variant_ptr) {

  std::scoped_lock<std::mutex> lock(add_variant_mutex_); // Write Locked

  return vcf_genome_ptr_->addVariant(variant_ptr);

}
