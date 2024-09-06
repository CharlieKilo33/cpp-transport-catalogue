#include "request_handler.h"

#include <utility>

RequestHandler::RequestHandler(
    transport::TransportCatalogue &transport_catalogue,
    MapRenderer &map_renderer, TransportRouter &transport_router)
    : transport_catalogue_(transport_catalogue), map_renderer_(map_renderer), transport_router_(transport_router) {}

void RequestHandler::AddRequests(
    std::pair<std::vector<detail::Stop>, std::vector<detail::Bus>>
    base_requests) {
  for (const auto &stop : base_requests.first) {
    transport_catalogue_.AddStop(stop);
  }

  for (const auto &stop : base_requests.first) {
    for (const auto &[stop_name, distance] : stop.distance) {
      auto from_stop_name = transport_catalogue_.FindStop(stop.name);
      auto to_stop_name = transport_catalogue_.FindStop(stop_name);
      transport_catalogue_.AddDistanceBetweenStops(from_stop_name, to_stop_name, distance);
    }
  }

  ParseBusses(base_requests.second);
}

void RequestHandler::ParseBusses(std::vector<detail::Bus> &busses) {
  for (auto &bus : busses) {
    std::vector<detail::Stop *> bus_stops;
    bus_stops.reserve(bus.stops_names.size() * (bus.is_roundtrip ? 1 : 2));
    for (const auto &stops_name : bus.stops_names) {
      auto stop_ptr = transport_catalogue_.FindStop(stops_name);

      if (stop_ptr) {
        bus_stops.emplace_back(stop_ptr);
      } else {
        std::cerr << "invalid stop_name";
        return;
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

std::vector<detail::Bus *> RequestHandler::GetBuses() {
  return transport_catalogue_.GetAllBuses();
}

void RequestHandler::AddRenderContext(RenderSettings render_settings) {
  RenderContext new_context;
  new_context.render_settings = std::move(render_settings);
  new_context.buses_route = GetBuses();
  map_renderer_.AddContext(new_context);
}

svg::Document RequestHandler::RenderMap() const {
  return map_renderer_.GetSVG();
}

std::pair<std::optional<graph::Router<double>::RouteInfo>, const TransportRouter::Graph &>
RequestHandler::GetOptimalRoute(const std::string_view from, const std::string_view to) const {
  auto route_info_opt = transport_router_.FindRoute(from, to);
  if (!route_info_opt) {
    return {std::nullopt, transport_router_.graph_};
  }
  return {*route_info_opt, transport_router_.graph_};
}

TransportRouter &RequestHandler::GetRouter() {
  return transport_router_;
}

