//
// Created by kellerberrin on 4/5/20.
//

#include "kgl_package_analysis.h"


namespace kgl = kellerberrin::genome;


bool kgl::PackageAnalysis::initializeAnalysis( const RuntimePackage& package,
                                               const std::shared_ptr<const AnalysisResources>& resource_ptr) const {


  active_analysis_.clear();
  for (auto const& analysis_id : package.analysisList()) {

    bool found = false;
    for (auto const& registered_analysis : registered_analysis_) {

      if (not registered_analysis) {

        ExecEnv::log().error("PackageAnalysis::initializeAnalysis, Registered Analysis is a NULL pointer");
        continue;

      }

      if (registered_analysis->ident() == analysis_id) {

        found = true;

        std::unique_ptr<VirtualAnalysis> analysis_ptr = registered_analysis->factory();

        auto result = runtime_contig_.analysisMap().find(analysis_id);

        // Analysis parameters found.
        if (result != runtime_contig_.analysisMap().end()) {

          // Get the named parameter blocks for this analysis.
          auto defined_parameters = runtime_contig_.activeParameterList().createParameterList(result->second.parameterMap());

          // Initialize.
          if (analysis_ptr->initializeAnalysis(runtime_contig_.workDirectory(), defined_parameters, resource_ptr)) {

            active_analysis_.emplace_back(std::move(analysis_ptr), true);

          } else {

            active_analysis_.emplace_back(std::move(analysis_ptr), false);  // Register but disable.
            ExecEnv::log().error("PackageAnalysis::initializeAnalysis, Failed to Initialize Analysis: {}, further analysis discarded", analysis_id);

          }

        } else {

          ExecEnv::log().error("PackageAnalysis::initializeAnalysis, Could not find Runtime Parameters for Analysis: {}, further analysis discarded", analysis_id);

        }

        break;

      }

    }

    if (not found) {

      ExecEnv::log().error("PackageAnalysis::initializeAnalysis, Could not find Analysis: {}. Analysis must be registered in PackageAnalysis::PackageAnalysis", analysis_id);

    }

  }

  return true;
}


bool kgl::PackageAnalysis::fileReadAnalysis(std::shared_ptr<const DataDB> file_data) const {

  for (auto& [analysis, active] : active_analysis_) {

    ExecEnv::log().info("File Read, Updating Analysis: {}", analysis->ident());

    if (active) {

      if (not analysis->fileReadAnalysis(file_data)) {

        ExecEnv::log().error("PackageAnalysis::fileReadAnalysis; Error Iteratively Updating Analysis: {}, disabled from further updates.", analysis->ident());
        active = false;
        return false;

      }

    } else {

      ExecEnv::log().warn("PackageAnalysis::fileReadAnalysis; Analysis: {} registered an error code and was disabled.", analysis->ident());

    }

  }

  ExecEnv::log().info("PackageAnalysis::fileReadAnalysis; Completed all file analysis");

  return true;

}


bool kgl::PackageAnalysis::iterationAnalysis() const {

  for (auto& [analysis, active] : active_analysis_) {

    ExecEnv::log().info("Updating Analysis: {}", analysis->ident());

    if (active) {

      if (not analysis->iterationAnalysis()) {

        ExecEnv::log().error("PackageAnalysis::iterationAnalysis; Error Iteratively Updating Analysis: {}, disabled from further updates.", analysis->ident());
        active = false;
        return false;

      }

    } else {

      ExecEnv::log().warn("PackageAnalysis::iterationAnalysis; Analysis: {} registered an error code and was disabled.", analysis->ident());

    }

  }

  return true;

}


bool kgl::PackageAnalysis::finalizeAnalysis() const {


  for (auto& [analysis, active] : active_analysis_) {

    if (active) {

      if (not analysis->finalizeAnalysis()) {

        ExecEnv::log().error("PackageAnalysis::initializeAnalysis, Error Finalizing Updating Analysis: {}", analysis->ident());
        active = false;
        return false;

      }

    } else {

      ExecEnv::log().warn("PackageAnalysis::finalizeAnalysis; Analysis: {} registered an error code and was disabled.", analysis->ident());

    }

  }

  return true;

}
