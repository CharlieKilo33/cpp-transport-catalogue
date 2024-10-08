#pragma once
#include <deque>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>

#include "geo.h"
#include "domain.h"

// namespace detail

class Hasher {
 public:
  std::size_t operator()(
      const std::pair<detail::Stop *, detail::Stop *> &p) const {
    std::size_t h1 = std::hash<detail::Stop *>()(p.first);
    std::size_t h2 = std::hash<detail::Stop *>()(p.second);
    return h1 ^ h2;
  }
};

namespace transport {
using namespace detail;

class TransportCatalogue {
 public:
  void AddStop(const Stop &stop);
  void AddBus(const Bus &bus);
  Stop *FindStop(std::string_view name) const;
  const Bus *GetBusInfo(std::string_view number) const;
  std::set<std::string_view> GetBusesThroughStop(std::string_view stop_name) const;
  void AddDistanceBetweenStops(Stop *from_stop, Stop *to_stop, double distance);
  std::pair<double, double> GetDistanceBetweenStops(Stop *from_stop, Stop *to_stop) const;
  std::vector<Bus *> GetAllBuses() const;
  std::vector<Stop *> GetSortedStops() const;

 private:
  class DistanceHasher {
   public:
    size_t operator()(
        const std::pair<detail::Stop *, detail::Stop *> &element) const {
      size_t hash = std::hash<const void *>{}(element.first);
      hash += std::hash<const void *>{}(element.second);
      return hash;
    }
  };

  std::deque<Stop> stops_;
  std::unordered_map<std::string_view, Stop *> stopname_to_stop_;
  std::deque<Bus> buses_;
  std::unordered_map<std::string_view, Bus *> busname_to_bus_;
  std::unordered_map<std::pair<Stop *, Stop *>, double, Hasher> distance_;
  std::unordered_map<const Stop *, std::vector<Bus *>> buses_through_stop_;
  std::unordered_map<std::pair<Stop *, Stop *>, double, DistanceHasher>
      distances_between_bus_stops_;
  size_t GetUniqueStations(std::string_view bus_number) const;
};
}  // namespace transport