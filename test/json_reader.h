#pragma once

#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "domain.h"
#include "json.h"
#include "transport_catalogue.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из
 * JSON, а также код обработки запросов к базе и формирование массива ответов в
 * формате JSON
 */

struct StatRequest{
  int id = 0;
  std::string type;
  std::string name;
};

class JsonReader{
 public:
  JsonReader(json::Document&  doc, transport::TransportCatalogue& transport_catalogue);

  std::pair<std::vector<detail::Stop>, std::vector<detail::Bus>> GetBaseRequests();
  [[maybe_unused]] std::vector<StatRequest> GetStatRequests();
  void ParseStatRequests(std::ostream& out);
  static std::map<std::string, json::Node> PrintBus(const detail::Bus* bus, int id);
  std::map<std::string, json::Node> PrintStop(detail::Stop* stop, int id);

 private:
  void LoadBaseRequests();
  void LoadStatRequests();
  static std::vector<std::string> ParseStopNames(const json::Node& array);
  std::pair<std::vector<detail::Stop>, std::vector<detail::Bus>> base_requests_;
  std::vector<StatRequest> stat_requests_;
  json::Document& doc_;
  transport::TransportCatalogue& transport_catalogue_;
};