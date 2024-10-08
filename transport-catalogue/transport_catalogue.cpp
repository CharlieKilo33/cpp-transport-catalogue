#include "transport_catalogue.h"

#include <iostream>
#include <unordered_set>

using namespace transport;

void TransportCatalogue::AddStop(const Stop &stop) {
  stops_.push_back(stop);
  Stop *added_stop = &stops_.back();
  stopname_to_stop_[added_stop->name] = added_stop;
}

void TransportCatalogue::AddBus(const Bus &bus) {
  buses_.push_back(bus);
  Bus *added_bus = &buses_.back();
  double length_between_bus_stops = 0;
  double length = 0;
  std::unordered_set<std::string_view> unique_stops;

  for (size_t i = 0; i < added_bus->stops.size() - 1; ++i) {
    auto &stop_i = added_bus->stops[i];
    auto &stop_i_plus_1 = added_bus->stops[i + 1];
    unique_stops.insert(stop_i->name);
    unique_stops.insert(stop_i_plus_1->name);
    auto distance_pair = GetDistanceBetweenStops(stop_i, stop_i_plus_1);
    length_between_bus_stops += distance_pair.first;
    length += distance_pair.second;
    if (i == added_bus->stops.size() - 2) {
      buses_through_stop_[stop_i_plus_1].push_back(added_bus);
    }
    buses_through_stop_[stop_i].push_back(added_bus);
  }
  busname_to_bus_[added_bus->number] = added_bus;
  added_bus->length_between_bus_stops = length_between_bus_stops;
  added_bus->curvature = added_bus->length_between_bus_stops / length;
  added_bus->unique_stops = GetUniqueStations(added_bus->number);
}

Stop *TransportCatalogue::FindStop(std::string_view name) const {
  return stopname_to_stop_.count(name) ? stopname_to_stop_.at(name) : nullptr;
}

const Bus *TransportCatalogue::GetBusInfo(std::string_view number) const {
  if (!busname_to_bus_.count(number)) {
    return nullptr;
  }
  Bus *bus = busname_to_bus_.at(number);
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

std::set<std::string_view> TransportCatalogue::GetBusesThroughStop(
    std::string_view stop_name) const {
  if (FindStop(stop_name) == nullptr) {
    std::cerr << "no stop";
    return {};
  }
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

void TransportCatalogue::AddDistanceBetweenStops(Stop *from_stop, Stop *to_stop,
                                                 double distance) {
  distances_between_bus_stops_[{from_stop, to_stop}] = distance;
  auto old_distance =
      ComputeDistance(from_stop->coordinates, to_stop->coordinates);
  distance_[{from_stop, to_stop}] = old_distance;
  distance_[{to_stop, from_stop}] = old_distance;
}

std::pair<double, double> TransportCatalogue::GetDistanceBetweenStops(
    detail::Stop *from_stop, detail::Stop *to_stop) const {
  auto distance = distances_between_bus_stops_.find({from_stop, to_stop});
  if (distance == distances_between_bus_stops_.end()) {
    return {distances_between_bus_stops_.at({to_stop, from_stop}), distance_.at({from_stop, to_stop})};
  }
  return {distance->second, distance_.at({from_stop, to_stop})};
}

std::vector<Bus *> TransportCatalogue::GetAllBuses() const{
  std::vector<Bus *> buses;
  buses.reserve(busname_to_bus_.size());
  for (const auto &[busname_to_stop, bus] : busname_to_bus_) {
    buses.push_back(bus);
  }
  return buses;
}


std::vector<Stop*> TransportCatalogue::GetSortedStops() const {
  std::vector<Stop*> stops;
  stops.reserve(stopname_to_stop_.size());
  for (const auto &[stopname_to_stop, stop] : stopname_to_stop_) {
    stops.push_back(stop);
  }
  return stops;
}