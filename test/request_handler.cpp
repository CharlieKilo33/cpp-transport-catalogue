#include "request_handler.h"

#include <utility>

RequestHandler::RequestHandler(
    transport::TransportCatalogue& transport_catalogue,
    std::pair<std::vector<detail::Stop>, std::vector<detail::Bus>>
        base_requests)
    : transport_catalogue_(transport_catalogue), base_requests_(base_requests) {
  for (const auto& stop : base_requests_.first) {
    transport_catalogue_.AddStop(stop);
  }

  for (const auto& stop : base_requests_.first) {
    for (const auto& [stop_name, distance] : stop.distance) {
      transport_catalogue_.AddDistanceBetweenStops(stop.name, stop_name, distance);
    }
  }

  ParseBusses(base_requests.second);
}

void RequestHandler::ParseBusses(std::vector<detail::Bus>& busses) {
  for (auto& bus : busses) {
    std::vector<detail::Stop*> bus_stops;
    bus_stops.reserve(bus.stops_names.size() * (bus.is_roundtrip ? 1 : 2));
    for (const auto& stops_name : bus.stops_names) {
      auto stop_ptr = transport_catalogue_.FindStop(stops_name);
      if (stop_ptr) {
        bus_stops.emplace_back(stop_ptr);
      } else {
        throw std::runtime_error("invalid stop_name");
      }
    }
    if (!bus.is_roundtrip) {
      for (int i = bus.stops_names.size() - 2; i >= 0; --i) {
        bus_stops.push_back(bus_stops[i]);
      }
    }
    bus.stops = std::move(bus_stops);
    transport_catalogue_.AddBus(bus);
  }
}
