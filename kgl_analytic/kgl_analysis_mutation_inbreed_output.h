//
// Created by kellerberrin on 30/11/20.
//

#ifndef KGL_ANALYSIS_MUTATION_INBREED_OUTPUT_H
#define KGL_ANALYSIS_MUTATION_INBREED_OUTPUT_H


#include "kgl_variant_db_phased.h"
#include "kgl_ped_parser.h"
#include "kgl_analysis_mutation_inbreed_calc.h"


namespace kellerberrin::genome {   //  organization::project level namespace


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Utility Object to hold inbreeding analysis results as they are generated.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The inbreeding analysis results and the parameter object used to generate them.
using ResultMapPair = std::pair<InbreedingParameters, ResultsMap>;

class InbreedingOutputResults {

public:

  explicit InbreedingOutputResults(const std::string& result_ident) : results_identifier_(result_ident) {}
  ~InbreedingOutputResults() = default;

  [[nodiscard]] const std::string& identifier() const { return results_identifier_; }
  [[nodiscard]] const std::vector<ResultMapPair>& resultsVector() const { return results_vector_; }

  void insertResults(const ResultMapPair& results) { results_vector_.push_back(results); }
  void identifier(const std::string& result_ident) {  results_identifier_ = result_ident; }

private:

  std::string results_identifier_;
  std::vector<ResultMapPair> results_vector_;

};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Format and output results to a csv file.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////




class InbreedingOutput  {

public:

  InbreedingOutput() = delete;
  ~InbreedingOutput() = delete;


  // Write the analysis results to a CSV file.
  static bool writeResults( const ResultsMap& genome_results_map,
                            const GenomePEDData& ped_data,
                            const std::string& output_file_name,
                            InbreedingParameters& parameters);

  // Write the analysis results to a CSV file.
  static bool writeColumnResults( const ColumnMap& column_results,
                                  const GenomePEDData& ped_data,
                                  const std::string& output_file_name,
                                  InbreedingParameters& parameters);

  static bool writeSynResults( const ResultsMap& genome_results_map,
                               InbreedingParameters& parameters);



private:

  constexpr static const char DELIMITER_ = ',';
  constexpr static const char* FILE_EXT_ = ".csv";

  // Check that each column has the same genome structure.
  [[nodiscard]] static bool verifyColumnMap(const ColumnMap& column_map);

};



} // namespace


#endif //KGL_ANALYSIS_MUTATION_INBREED_OUTPUT_H