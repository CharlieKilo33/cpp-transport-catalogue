#include "transport_catalogue.h"

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

  double length = 0;
  for (size_t i = 0; i < added_bus->stops.size() - 1; ++i) {
    length += AddDistance(added_bus->stops[i], added_bus->stops[i + 1]);
    if(i == added_bus->stops.size() - 2){
      buses_through_stop_[added_bus->stops[i + 1]].push_back(added_bus);
    }
    buses_through_stop_[added_bus->stops[i]].push_back(added_bus);
  }
  added_bus->length = length;

  busname_to_bus_[added_bus->number] = added_bus;
}

Stop *TransportCatalogue::FindStop(std::string_view name) {
  return stopname_to_stop_.count(name) ? stopname_to_stop_.at(name) : nullptr;
}

const Stop *TransportCatalogue::FindStop(std::string_view name) const {
  return stopname_to_stop_.count(name) ? stopname_to_stop_.at(name) : nullptr;
}

Bus *TransportCatalogue::FindBus(std::string_view number) {
  return busname_to_bus_.count(number) ? busname_to_bus_.at(number) : nullptr;
}

const Bus *TransportCatalogue::FindBus(std::string_view number) const {
  return busname_to_bus_.count(number) ? busname_to_bus_.at(number) : nullptr;
}

std::vector<Stop *> TransportCatalogue::GetBusInfo(std::string_view number) const {
  if (busname_to_bus_.count(number) == 0) {
    return {};
  }
  return busname_to_bus_.at(number)->stops;
}

size_t TransportCatalogue::GetUniqueStations(std::string_view bus_number) const {
  auto bus = FindBus(bus_number);
  std::unordered_set<std::string_view> unique_stations;
  for (auto stops : bus->stops) {
    unique_stations.insert(stops->name);
  }
  return unique_stations.size();
}

double TransportCatalogue::AddDistance(Stop *first, Stop *second) {
  double res = ComputeDistance(first->coordinates, second->coordinates);
  distance_[{first, second}] = res;
  return res;
}
std::set<std::string_view> TransportCatalogue::GetBusesThroughStop(std::string_view stop_name) const  {
  if(buses_through_stop_.count(FindStop(stop_name)) == 0){
    return {};
  } else {
    std::set<std::string_view> buses_through_stop;
    for(auto buses : buses_through_stop_.at(FindStop(stop_name))){
      buses_through_stop.insert(buses->number);
    }
    return buses_through_stop;
  }
}


