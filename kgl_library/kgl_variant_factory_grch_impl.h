//
// Created by kellerberrin on 20/4/20.
//

#ifndef KGL_VARIANT_FACTORY_GRCH_IMPL_H
#define KGL_VARIANT_FACTORY_GRCH_IMPL_H




#include "kel_utility.h"
#include "kgl_variant_file_impl.h"
#include "kgl_variant_factory_readvcf_impl.h"
#include "kgl_variant_factory_record_vcf_impl.h"



namespace kellerberrin::genome {   //  organization level namespace


class GrchVCFImpl : public VCFReaderMT {

public:

  GrchVCFImpl(std::shared_ptr<UnphasedGenome> vcf_genome_ptr,
              std::shared_ptr<const GenomeDatabase> genome_db_ptr,
              const std::string &vcf_file_name) : VCFReaderMT(vcf_file_name),
                                                  vcf_genome_ptr_(std::move(vcf_genome_ptr)),
                                                  genome_db_ptr_(std::move(genome_db_ptr)){

  }

  ~GrchVCFImpl() override = default;

  void ProcessVCFRecord(size_t vcf_record_count, const VcfRecord &vcf_record) override;

  void processVCFHeader(const VcfHeaderInfo &header_info) override;

  void readParseVCFImpl();


private:

  constexpr static const size_t VARIANT_REPORT_INTERVAL_ = 100000;

  std::shared_ptr<UnphasedGenome> vcf_genome_ptr_;
  std::shared_ptr<const GenomeDatabase> genome_db_ptr_;

// Progress counters.

  size_t vcf_variant_count_{0};
  std::atomic<uint64_t> record_count_{0};
  size_t variant_count_{0};
  mutable std::mutex add_variant_mutex_;

  std::map<ContigId_t, std::pair<ContigSize_t, size_t>> contig_count_;

  bool createAddVariant( const GenomeId_t& genome_name,
                         const ContigId_t& contig_id,
                         ContigOffset_t contig_offset,
                         const std::string& reference_text,
                         const std::string& alternate_text,
                         const std::shared_ptr<const VariantEvidence> evidence_ptr);

  bool addThreadSafeVariant(std::shared_ptr<const Variant>& variant_ptr);

};

} // namespace

#endif //KGL_KGL_VARIANT_FACTORY_GRCH_IMPL_H
