#include "transport_router.h"

void TransportRouter::AddSettings(const int wait_time,
                                  const double velocity) {
  bus_wait_time_ = wait_time;
  bus_velocity_ = velocity;
  BuildGraph();
}

void TransportRouter::BuildGraph() {
  stop_name_to_vertex_id_.clear();
  graph_.Resize(catalogue_.GetSortedStops().size() * 2);
  AddStopsToGraph();
  AddBusesToGraph();
  router_ = std::make_unique<graph::Router<double>>(graph_);
}

void TransportRouter::AddStopsToGraph() {
  stop_name_to_vertex_id_.reserve(catalogue_.GetSortedStops().size());
  graph::VertexId vertex_id = 0;

  for (const auto &stop : catalogue_.GetSortedStops()) {
    stop_name_to_vertex_id_[stop->name] = vertex_id;
    graph_.AddEdge({
                       stop->name,
                       0,
                       vertex_id,
                       ++vertex_id,
                       static_cast<double>(bus_wait_time_)
                   });
    ++vertex_id;
  }
}


void TransportRouter::AddBusesToGraph() {
  for (const auto &bus_item : catalogue_.GetAllBuses()) {
    const detail::Bus *bus_info = bus_item;
    const std::vector<detail::Stop *> &stops = bus_info->stops;
    size_t stops_count = stops.size();

    for (size_t i = 0; i < stops_count; ++i) {
      int dist_sum = 0;
      int dist_sum_inverse = 0;

      for (size_t j = i + 1; j < stops_count; ++j) {
        const detail::Stop *stop_from = stops[i];
        const detail::Stop *stop_to = stops[j];

        dist_sum += catalogue_.GetDistanceBetweenStops(stops[j - 1], stops[j]).first;
        dist_sum_inverse += catalogue_.GetDistanceBetweenStops(stops[j], stops[j - 1]).first;

        const double weight = static_cast<double>(dist_sum) / (bus_velocity_ * (100.0 / 6.0));
        graph_.AddEdge({
                           bus_info->number,
                           j - i,
                           stop_name_to_vertex_id_.at(stop_from->name) + 1,
                           stop_name_to_vertex_id_.at(stop_to->name),
                           weight
                       });

        if (!bus_info->is_roundtrip) {
          const double weight_inverse = static_cast<double>(dist_sum_inverse) / (bus_velocity_ * (100.0 / 6.0));
          graph_.AddEdge({
                             bus_info->number,
                             j - i,
                             stop_name_to_vertex_id_.at(stop_to->name) + 1,
                             stop_name_to_vertex_id_.at(stop_from->name),
                             weight_inverse
                         });
        }
      }
    }
  }
}

std::optional<graph::Router<double>::RouteInfo> TransportRouter::FindRoute(const std::string_view stop_from,
                                                                           const std::string_view stop_to) const {
  try {
    return router_.get()->BuildRoute(GetVertexId(stop_from), GetVertexId(stop_to));
  }
  catch (const std::invalid_argument &) {
    return std::nullopt;
  }
}

