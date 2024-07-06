#pragma once

#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "domain.h"
#include "json.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"

struct StatRequest {
  int id = 0;
  std::string type;
  std::string name;
};

class JsonReader {
 public:
  JsonReader(transport::TransportCatalogue& transport_catalogue,
             RequestHandler& request_handler, json::Document& doc);

  void ParseStatRequests(std::ostream& out);

 private:
  void LoadBaseRequests();
  void LoadRenderSettings();
  void LoadStatRequests();
  static std::vector<std::string> ParseStopNames(const json::Node& array);
  static std::map<std::string, json::Node> GetMapBuses(const detail::Bus* bus,
                                                       int id);
  std::map<std::string, json::Node> GetMapStops(detail::Stop* stop, int id);
  std::map<std::string, json::Node> GetMapMap(int id);
  std::vector<StatRequest> stat_requests_;
  transport::TransportCatalogue& transport_catalogue_;
  RequestHandler& request_handler_;
  json::Document& doc_;
};