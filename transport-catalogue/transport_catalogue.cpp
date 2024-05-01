#include "transport_catalogue.h"

#include <unordered_set>

using namespace transport;

void TransportCatalogue::AddStop(const Stop& stop) {
  stops_.push_back(stop);
  Stop* added_stop = &stops_.back();
  stopname_to_stop_[added_stop->name] = added_stop;
}

void TransportCatalogue::AddBus(const Bus& bus) {
  buses_.push_back(bus);
  Bus* added_bus = &buses_.back();
  double length_between_bus_stops = 0;
  double length = 0;

  for (size_t i = 0; i < added_bus->stops.size() - 1; ++i) {
    length_between_bus_stops +=
        GetDistanceBetweenStops(added_bus->stops[i], added_bus->stops[i + 1]);
    length += AddDistance(added_bus->stops[i], added_bus->stops[i + 1]);
    if (i == added_bus->stops.size() - 2) {
      buses_through_stop_[added_bus->stops[i + 1]].push_back(added_bus);
    }
    buses_through_stop_[added_bus->stops[i]].push_back(added_bus);
  }
  busname_to_bus_[added_bus->number] = added_bus;
  added_bus->length_between_bus_stops = length_between_bus_stops;
  added_bus->curvature = length_between_bus_stops / length;
  added_bus->unique_stops = GetUniqueStations(added_bus->number);
}

Stop* TransportCatalogue::FindStop(std::string_view name) {
  return stopname_to_stop_.count(name) ? stopname_to_stop_.at(name) : nullptr;
}

const Stop* TransportCatalogue::FindStop(std::string_view name) const {
  return stopname_to_stop_.count(name) ? stopname_to_stop_.at(name) : nullptr;
}

const Bus* TransportCatalogue::GetBusInfo(std::string_view number) const {
  if (!busname_to_bus_.count(number)) {
    return nullptr;
  }
  Bus* bus = busname_to_bus_.at(number);
  return bus;
}

size_t TransportCatalogue::GetUniqueStations(
    std::string_view bus_number) const {
  auto bus = busname_to_bus_.at(bus_number);
  std::unordered_set<std::string_view> unique_stations;
  for (auto stops : bus->stops) {
    unique_stations.insert(stops->name);
  }
  return unique_stations.size();
}

double TransportCatalogue::AddDistance(Stop* first, Stop* second) {
  double res = ComputeDistance(first->coordinates, second->coordinates);
  distance_[{first, second}] = res;
  return res;
}

std::set<std::string_view> TransportCatalogue::GetBusesThroughStop(
    std::string_view stop_name) const {
  if (buses_through_stop_.count(FindStop(stop_name)) == 0) {
    return {};
  } else {
    std::set<std::string_view> buses_through_stop;
    for (auto buses : buses_through_stop_.at(FindStop(stop_name))) {
      buses_through_stop.insert(buses->number);
    }
    return buses_through_stop;
  }
}

void TransportCatalogue::AddDistanceBetweenStops(std::string_view from_stop,
                                                 std::string_view to_stop,
                                                 double distance) {
  Stop* stop1 = stopname_to_stop_.at(from_stop);
  Stop* stop2 = stopname_to_stop_.at(to_stop);
  distances_between_bus_stops_[{stop1, stop2}] = distance;
}

double TransportCatalogue::GetDistanceBetweenStops(
    detail::Stop* from_stop, detail::Stop* to_stop) const {
  auto distance = distances_between_bus_stops_.find({from_stop, to_stop});
  if (distance == distances_between_bus_stops_.end()) {
    return distances_between_bus_stops_.at({to_stop, from_stop});
  }
  return distance->second;
}