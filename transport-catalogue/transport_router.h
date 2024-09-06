#pragma once
#include "json.h"
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"
#include <utility>
#include <memory>

class TransportRouter {
 public:
  using Graph = graph::DirectedWeightedGraph<double>;
  using Router = std::unique_ptr<graph::Router<double>>;

  Graph graph_;

  explicit TransportRouter(const transport::TransportCatalogue &transport_catalogue)
      : catalogue_(transport_catalogue) {};

  void AddSettings(int wait_time, double velocity);

  std::optional<graph::Router<double>::RouteInfo> FindRoute(std::string_view stop_from, std::string_view stop_to) const;

 private:
  int bus_wait_time_ = 0;
  double bus_velocity_ = 0.0;
  Router router_;
  const transport::TransportCatalogue &catalogue_;
  std::unordered_map<std::string, graph::VertexId> stop_name_to_vertex_id_;

  void BuildGraph();
  void AddStopsToGraph();
  void AddBusesToGraph();

  graph::VertexId GetVertexId(const std::string_view stop_name) const {
    auto it = stop_name_to_vertex_id_.find(std::string(stop_name));
    if (it != stop_name_to_vertex_id_.end()) {
      return it->second;
    }
    throw std::invalid_argument("Stop name not found");
  }

};