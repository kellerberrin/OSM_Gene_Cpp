//
// Created by kellerberrin on 12/12/19.
//

#include "kpl_tree.h"
#include "kpl_xstrom.h"

#include <boost/format.hpp>

#include <sstream>

namespace kpl = kellerberrin::phylogenetic;

// member function bodies go here

kpl::Tree::Tree(unsigned num_nodes)
{

  clear();

  if (num_nodes > 0) {

    _nodes.resize(num_nodes);

  }

}


void kpl::Tree::clear() {

  _root = nullptr;
  _nodes.clear();
  _preorder.clear();
  _levelorder.clear();

}


// Only used in TreeManip.
kpl::Node::PtrNode kpl::Tree::getNode(size_t node_index) {

  if (node_index >= _nodes.size()) {

    throw XStrom(boost::str(boost::format("Tree has %d nodes, Invalid node index %d") % _nodes.size() % node_index));

  }

  return &(_nodes[node_index]);

}

// Only used in TreeManip.
kpl::Node::ConstPtrNode kpl::Tree::getConstNode(size_t node_index) const {

  if (node_index >= _nodes.size()) {

    throw XStrom(boost::str(boost::format("Tree has %d nodes, Invalid node index %d") % _nodes.size() % node_index));

  }

  return &(_nodes[node_index]);

}


kpl::Node::ConstPtrVector kpl::Tree::getConstNodes() const {

  Node::ConstPtrVector nodes;

  for (auto& node : _nodes) {

    nodes.push_back(&node);

  }

  return nodes;

}


kpl::Node::PtrVector kpl::Tree::getNodes() {

  Node::PtrVector nodes;

  for (auto& node : _nodes) {

    nodes.push_back(&node);

  }

  return nodes;

}


bool kpl::Tree::isRooted() const {

  return static_cast<bool>(_root);

}


unsigned kpl::Tree::numLeaves() const {

  return _nleaves;

}


unsigned kpl::Tree::numInternals() const {

  return _ninternals;

}


unsigned kpl::Tree::numNodes() const {

  return (unsigned) _nodes.size();

}


std::string kpl::Tree::treeDescription() const {

  std::stringstream ss;

  ss << "\n****************\n";
  ss << "Is Rooted:" << (isRooted() ? "ROOTED" : "NotRooted") << '\n';
  if (isRooted()) {

    ss << "Rooted Node:" << Node::printNode(getConstRoot()) << '\n';

  }
  ss << "Tree Node Count:" << getConstNodes().size() << '\n';
  for (auto node : getConstNodes()) {

    ss << Node::printNode( node) << '\n';

  }
  ss << "Leaves Count:" << numLeaves() << '\n';
  ss << "Internal Nodes Count:" << numInternals() << '\n';
  ss << "Unused Nodes Count:" << getUnUsed().size() << '\n';
  ss << "Preorder Nodes Count:" << getConstPreOrder().size() << '\n';
  for (auto node : getConstPreOrder()) {

//    ss << static_cast<const void *>(node) << '\n';

  }
  ss << "Level Nodes Count:" << getConstLevelOrder().size() << '\n';
  for (auto node : getConstLevelOrder()) {

//    ss << static_cast<const void *>(node) << '\n';

  }
  ss << "****************\n" << std::endl;

  return ss.str();

}

