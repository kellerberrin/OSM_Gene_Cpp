//
// Created by kellerberrin on 10/11/18.
//

#ifndef KGL_EPIGENETIC_MOTIF_H
#define KGL_EPIGENETIC_MOTIF_H


#include "kgl_genome_db.h"


namespace kellerberrin {   //  organization level namespace
namespace genome {   // project level namespace


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Find Promoter sequences and Transcription Start Sequences
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////





class PromoterMotif {

public:

  PromoterMotif() = default;
  ~PromoterMotif() = default;

  static void displayTFFMotif(std::shared_ptr<const GenomeDatabase> genome_db_ptr, const std::string& file_name, const char delimiter);

private:

  static void headerTFFMotif(std::ofstream& motif_file, const char delimiter);


};





}   // namespace genome
}   // namespace kellerberrin





#endif //KGL_EPIGENETIC_MOTIF_H
