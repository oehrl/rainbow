#include "rainbow/photon_map.hpp"

namespace rainbow {

void PhotonMap::Build(Photon* begin, Photon* end, size_t max_children) {
  max_children_ = max_children;
  const size_t photon_count = end - begin;
  const double minimum_number_of_leaf_nodes =
      static_cast<double>(photon_count) / static_cast<double>(max_children);
  const double minimum_tree_depth = std::log2(minimum_number_of_leaf_nodes);
  const double tree_depth = std::ceil(minimum_tree_depth);
  const double number_of_nodes = std::pow(2.0, tree_depth + 1.0) - 1.0;
  nodes_.resize(static_cast<size_t>(number_of_nodes));
  InsertIntoNode(0, 0, begin, end);
}

void PhotonMap::InsertIntoNode(int node_index, int plane_axis, Photon* begin,
                               Photon* end) {
  assert(node_index < nodes_.size());
  Node& current_node = nodes_[node_index];

  const size_t photon_count = end - begin;
  if (photon_count <= max_children_) {
    current_node.begin = begin;
    current_node.end = end;
  } else {
    std::sort(begin, end, [plane_axis](const Photon& lhs, const Photon& rhs) {
      return lhs.position[plane_axis] < rhs.position[plane_axis];
    });

    const size_t median_index = photon_count / 2;
    Photon* median = begin + median_index;

    current_node.begin = nullptr;
    current_node.end = nullptr;
    current_node.plane_axis = plane_axis;
    current_node.plane_position = median->position[plane_axis];

    int next_plane_axis = (plane_axis + 1) % 3;
    InsertIntoNode(GetLeftChild(node_index), next_plane_axis, begin, median);
    InsertIntoNode(GetRightChild(node_index), next_plane_axis, median, end);
  }
}

void PhotonMap::GetKNearestNeighbors(Vector3 position, int k,
                                     std::vector<Photon>* photons) const {
  assert(photons != nullptr);
  photons->resize(0);
  photons->reserve(3 * max_children_);
  GetKNearestNeighbors(0, position, k, photons);
}

void PhotonMap::GetKNearestNeighbors(int node_index, Vector3 position, int k,
                                     std::vector<Photon>* photons) const {
  assert(node_index < nodes_.size());
  const auto& node = nodes_[node_index];
  if (IsLeafNode(node)) {
    const auto node_count = node.end - node.begin;
    photons->reserve(photons->size() + node_count);
    photons->insert(photons->end(), node.begin, node.end);
  } else {
    const float signed_distance_to_plane =
        position[node.plane_axis] - node.plane_position;
    const float distance_to_plance_squared =
        signed_distance_to_plane * signed_distance_to_plane;
    if (position[node.plane_axis] < node.plane_position) {
      GetKNearestNeighbors(GetLeftChild(node_index), position, k, photons);
      float max_photon_distance_squared = Infinity<float>();
      if (photons->size() > 0) {
        const Vector3 furthest_photon_position = photons->back().position;
        max_photon_distance_squared =
            SquaredLength(furthest_photon_position - position);
      }
      if (photons->size() < k ||
          max_photon_distance_squared > distance_to_plance_squared) {
        // TODO: we probably do not need to add all of them here
        GetKNearestNeighbors(GetRightChild(node_index), position, k, photons);
      }
    } else {
      GetKNearestNeighbors(GetRightChild(node_index), position, k, photons);
      float max_photon_distance_squared = Infinity<float>();
      if (photons->size() > 0) {
        const Vector3 furthest_photon_position = photons->back().position;
        max_photon_distance_squared =
            SquaredLength(furthest_photon_position - position);
      }
      if (photons->size() < k ||
          max_photon_distance_squared > distance_to_plance_squared) {
        // TODO: we probably do not need to add all of them here
        GetKNearestNeighbors(GetLeftChild(node_index), position, k, photons);
      }
    }
  }

  std::sort(photons->begin(), photons->end(),
            [position](const Photon& lhs, const Photon& rhs) {
              return SquaredLength(lhs.position - position) <
                     SquaredLength(rhs.position - position);
            });
  if (photons->size() > k) {
    photons->resize(k);
  }
}

}  // namespace rainbow