#pragma once
#include <list>

#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

class RequestHandler {
 public:
  RequestHandler(transport::TransportCatalogue &transport_catalogue,
                 MapRenderer &map_renderer, TransportRouter &transport_router);
  void AddRequests(
      std::pair<std::vector<detail::Stop>, std::vector<detail::Bus>>
      base_requests);
  void AddRenderContext(RenderSettings render_settings);
  TransportRouter &GetRouter();
  std::vector<detail::Bus *> GetBuses();
  [[nodiscard]] svg::Document RenderMap() const;
  [[nodiscard]] std::pair<std::optional<graph::Router<double>::RouteInfo>,
                          const TransportRouter::Graph &> GetOptimalRoute(std::string_view from,
                                                                          std::string_view to) const;

 private:
  void ParseBusses(std::vector<detail::Bus> &busses);
  transport::TransportCatalogue &transport_catalogue_;
  MapRenderer &map_renderer_;
  TransportRouter &transport_router_;
};