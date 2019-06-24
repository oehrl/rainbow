#pragma once

#include <vector>
#include "rainbow/data_structures.hpp"

namespace rainbow {

class PhotonMap {
 public:
  // TODO: use span instead of begin/end
  void Build(Photon* begin, Photon* end, size_t max_children = 100);
  void GetKNearestNeighbors(Vector3 position, int k,
                            std::vector<Photon>* photons) const;

 private:
  struct Node {
    // TODO: use span instead of begin/end
    Photon* begin;
    Photon* end;
    float plane_position;
    int plane_axis;
  };

  size_t max_children_;
  std::vector<Node> nodes_;

  static inline bool IsLeafNode(const Node& node) {
    return node.begin != nullptr && node.end != nullptr;
  }

  inline bool IsLeafNode(int node_index) const {
    assert(node_index < nodes_.size());
    return IsLeafNode(nodes_[node_index]);
  }

  void InsertIntoNode(int node_index, int plane_axis, Photon* begin,
                      Photon* end);
  void GetKNearestNeighbors(int node_index, Vector3 position, int k,
                            std::vector<Photon>* photons) const;

  static inline int GetLeftChild(int index) { return 2 * index + 1; }
  static inline int GetRightChild(int index) { return 2 * index + 2; }
  static inline int GetParent(int index) {
    assert(index > 0);
    return (index - 1) / 2;
  }
};

}  // namespace rainbow