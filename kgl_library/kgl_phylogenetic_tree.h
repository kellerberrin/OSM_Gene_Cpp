//
// Created by kellerberrin on 23/05/19.
//

#ifndef KGL_PHYLOGENETIC_TREE_H
#define KGL_PHYLOGENETIC_TREE_H


#include <memory>
#include <map>
#include <vector>
#include <fstream>

#include "kgl_exec_env.h"


namespace kellerberrin {   //  organization level namespace
namespace genome {   // project level namespace


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Virtual Distance class implemented elsewhere that actually calculates the Phylo tree distance.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using DistanceType_t = double;

class VirtualDistanceNode {

public:

  VirtualDistanceNode() = default;
  VirtualDistanceNode(const VirtualDistanceNode&) = default;
  virtual ~VirtualDistanceNode() = default;

  // Classification functions
  // Function to tag the nodes. Override as necessary.
  virtual void writeNode(std::ostream& outfile) const = 0;
  // Pure Virtual calculates the distance between nodes.
  virtual DistanceType_t distance(std::shared_ptr<const VirtualDistanceNode> distance_node) const = 0;
  // Pure Virtual calculates the zero distance between nodes.
  // This function is only re-defined and used if the distance metric needs to set a particular
  // condition for a zero distance. Most distance metrics will not need to re-define this function.
  virtual DistanceType_t zeroDistance(std::shared_ptr<const VirtualDistanceNode>) const { return 1.0; }

private:

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tree Classification Nodes.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Used by the classification functions.
class PhyloNode;  // fwd.
using PhyloNodeVector = std::vector<std::shared_ptr<PhyloNode>>;
using OutNodes = std::multimap<DistanceType_t , std::shared_ptr<PhyloNode>>;

class PhyloNode {

public:

  explicit PhyloNode(std::shared_ptr<const VirtualDistanceNode> leaf) : leaf_(leaf), distance_(0.0) {}
  ~PhyloNode() = default;

  void addOutNode(std::shared_ptr<PhyloNode> node) {
    out_nodes_.insert(std::pair<DistanceType_t , std::shared_ptr<PhyloNode>>(node->distance(), node));
  }

  DistanceType_t distance() const { return distance_; }
  void distance(DistanceType_t update) { distance_ = update; }

  std::shared_ptr<const VirtualDistanceNode> leaf() const { return leaf_; }
  const OutNodes& getMap() const { return out_nodes_; }

  // Recursively counts the total number of leaf nodes.
  bool isLeaf() const { return getMap().empty(); }
  size_t leafNodeCount() const;

private:

  std::shared_ptr<const VirtualDistanceNode> leaf_;
  DistanceType_t distance_;
  OutNodes out_nodes_;

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Abstract Phylogenetic Tree
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class DistanceTree {

public:

  DistanceTree() {}
  virtual ~DistanceTree() = default;


  virtual void calculateTree(std::shared_ptr<PhyloNodeVector> node_vector_ptr) = 0;

  virtual bool writeNewick(const std::string &file_name) const = 0;

protected:


};




}   // namespace genome
}   // namespace kellerberrin






#endif //KGL_PHYLOGENETIC_TREE_H
