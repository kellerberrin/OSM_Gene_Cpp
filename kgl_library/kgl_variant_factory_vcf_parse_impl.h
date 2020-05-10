//
// Created by kellerberrin on 28/02/18.
//

#ifndef KGL_VARIANT_FACTORY_VCF_PARSE_IMPL_H
#define KGL_VARIANT_FACTORY_VCF_PARSE_IMPL_H



#include <map>

#include "kel_utility.h"
#include "kgl_genome_types.h"
#include "kgl_genome_db.h"
#include "kgl_variant_db.h"


namespace kellerberrin::genome {   //  organization level namespace


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VCF parser. Miscellaneous parser functions.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Structure to return VCF header information as a vector of pair<key, value>
using VcfHeaderInfo = std::vector<std::pair<std::string, std::string>>;
// A map of contigs.
using ActiveContigMap = std::map<ContigId_t, ContigSize_t>;

// Returned from the cigar functions.
enum class CigarEditType : char { UNCHANGED = 'M', INSERT = 'I', DELETE = 'D', CHANGED = 'X'};
using CigarEditItem = std::pair<size_t, CigarEditType>; // Used to specify edit as vector '1M,1X,3D,3I'.
using CigarVector = std::vector<CigarEditItem>;

using ActiveContigMap = std::map<ContigId_t, ContigSize_t>;
using VcfInfoKeyMap = std::map<std::string, std::string>;

class ParseVCFMiscImpl {

public:

  ParseVCFMiscImpl() = default;
  ~ParseVCFMiscImpl() = default;


  [[nodiscard]] static bool parseVcfHeader( std::shared_ptr<const GenomeReference> genome_db_ptr,
                                            const VcfHeaderInfo& header,
                                            ActiveContigMap& active_contig_map,
                                            bool cigar_required);

  [[nodiscard]] static bool parseCigar( const std::string& cigar,
                                        size_t& check_reference_size,
                                        size_t& check_alternate_size,
                                        std::vector<CigarEditItem>& parsed_cigar);

// tokenize a string
  [[nodiscard]] static bool tokenize(const std::string& parse_text, const std::string& separator_text, std::vector<std::string>& item_vector);

// tokenize a string using move semantics on parse text.
  [[nodiscard]] static bool tokenize(std::string&& parse_text, const std::string& separator_text, std::vector<std::string>& item_vector);

  // Generate a CIGAR from two sequences.
  [[nodiscard]] static std::string generateCigar(const std::string& reference, const std::string& alternate);

  // Generate a CIGAR from a cigar vector
  [[nodiscard]] static std::string generateCigar(const CigarVector& cigar_vector);

// Use edlib to generate a cigar vector.
  [[nodiscard]] static CigarVector generateEditVector(const std::string& reference, const std::string& alternate);

  // Given a reference count and a cigar vector compute a number that calculates the equivalent
  // size of the alternate string.
  // For UNCHANGED = 'M' and CHANGED = 'X' cigar items the reference_count and alternate count are incremented.
  // For INSERT = 'I' the alternate is incremented and the reference_count is not.
  // For DELETE = 'D' the reference count is incremented and the alternate is not.
  [[nodiscard]] static size_t alternateCount(size_t reference_count, const CigarVector& cigar_vector);


private:

  // assumes input "<key_1=value_1, ...,key_n=value_n>"
  [[nodiscard]] static bool tokenizeVcfHeaderKeyValues( const std::string& key_value_text,
                                                        VcfInfoKeyMap& key_value_pairs);

// Use edlib to generate a cigar string.
  static void generateEditString(const std::string& reference, const std::string& alternate, std::vector<CigarEditType>& edit_vector);

  constexpr static const char* HEADER_CONTIG_KEY_ = "CONTIG";
  constexpr static const char* ID_KEY_ = "ID";
  constexpr static const char* CONTIG_LENGTH_KEY_ = "LENGTH";
  constexpr static const char* HEADER_INFO_KEY_ = "INFO";
  constexpr static const char* ID_CIGAR_VALUE_ = "CIGAR";
  constexpr static const char* ID_READ_DEPTH_ = "DPB";
  constexpr static const char* ID_PROPORTION_ = "AF";


};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is an efficient single pass parser.
// In general std::string_view is work of the devil and a seg fault waiting to happen.
// But if the underlying string has the same guaranteed lifetime as the associated std::string_view(s) then a seg fault
// may not be inevitable.
using InfoParserMap = std::map<std::string_view, std::string_view>;
class VCFInfoParser {

public:

  // std::move the info string into this object for efficiency.
  explicit VCFInfoParser(std::string&& info) : info_(std::move(info)), info_view_(info_) {

    if (not parseInfo()) {

      ExecEnv::log().error("VCFInfoParser::VCFInfoParser, Problem parsing info field");

    }

  }
  ~VCFInfoParser() = default;

  [[nodiscard]] const InfoParserMap& getMap() const { return parsed_token_map_; }
  [[nodiscard]] const std::string& info() const { return info_; }
  [[nodiscard]] std::optional<std::string> getInfoField(const std::string& key) const;

private:

  std::string info_;
  std::string_view info_view_;
  InfoParserMap parsed_token_map_;

  constexpr static const char INFO_FIELD_DELIMITER_{';'};
  constexpr static const char INFO_VALUE_DELIMITER_{'='};

  [[nodiscard]] bool parseInfo();

};



}   // end namespace








#endif //KGL_VARIANT_FACTORY_VCF_PARSE_IMPL_H
