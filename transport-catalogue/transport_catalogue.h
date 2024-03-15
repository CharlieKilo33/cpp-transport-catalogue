#pragma once
#include <unordered_map>
#include <deque>
#include <vector>
#include <string>
#include <set>

#include "geo.h"

namespace detail {
struct Stop {
  Stop() = default;
  std::string name;
  Coordinates coordinates{};
};

struct Bus {
  Bus() = default;
  std::string number;
  std::vector<Stop *> stops;
  double length = 0;
};
}

class Hasher {
 public:
  std::size_t operator()(const std::pair<detail::Stop *, detail::Stop *> &p) const {
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
  double AddDistance(Stop *first, Stop *second);
  Stop *FindStop(std::string_view name);
  const Stop *FindStop(std::string_view name) const;
  Bus *FindBus(std::string_view number);
  const Bus *FindBus(std::string_view number) const;
  std::vector<Stop *> GetBusInfo(std::string_view number) const;
  size_t GetUniqueStations(std::string_view bus_number) const;
  std::set<std::string_view> GetBusesThroughStop(std::string_view stop_name) const;

 private:
  std::deque<Stop> stops_;
  std::unordered_map<std::string_view, Stop *> stopname_to_stop_;
  std::deque<Bus> buses_;
  std::unordered_map<std::string_view, Bus *> busname_to_bus_;
  std::unordered_map<std::pair<Stop *, Stop *>, double, Hasher> distance_;
  std::unordered_map<const Stop *, std::vector<Bus*>> buses_through_stop_;
};
}