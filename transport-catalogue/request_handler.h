#pragma once
#include <list>

#include "map_renderer.h"
#include "transport_catalogue.h"

class RequestHandler {
 public:
  RequestHandler(transport::TransportCatalogue& transport_catalogue,
                 MapRenderer& map_renderer);
  void AddRequests(
      std::pair<std::vector<detail::Stop>, std::vector<detail::Bus>>
          base_requests);
  void AddRenderContext(RenderSettings render_settings);
  std::vector<detail::Bus*> GetBuses();
  [[nodiscard]] svg::Document RenderMap() const;

 private:
  void ParseBusses(std::vector<detail::Bus>& busses);
  transport::TransportCatalogue& transport_catalogue_;
  MapRenderer& map_renderer_;
};