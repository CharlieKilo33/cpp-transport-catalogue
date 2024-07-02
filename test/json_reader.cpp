#include "json_reader.h"

#include <utility>

#include "domain.h"

JsonReader::JsonReader(json::Document& doc,
                       transport::TransportCatalogue& transport_catalogue)
    : doc_(doc), transport_catalogue_(transport_catalogue) {
  LoadBaseRequests();
  LoadStatRequests();
}

std::vector<std::string> JsonReader::ParseStopNames(const json::Node& array) {
  if (!array.IsArray()) {
    throw std::runtime_error("Not array");
  }
  std::vector<std::string> stops_names;
  stops_names.reserve(array.AsArray().size());
  for (const auto& node : array.AsArray()) {
    stops_names.emplace_back(node.AsString());
  }
  return stops_names;
}


void JsonReader::LoadBaseRequests() {
  auto base_requests = doc_.GetRoot().AsMap().at("base_requests");
  if (base_requests != nullptr) {
    if (base_requests.IsArray()) {
      std::vector<json::Node> temp_busses;
      std::vector<detail::Stop> stops;
      std::vector<detail::Bus> busses;
      stops.reserve(base_requests.AsArray().size());
      busses.reserve(base_requests.AsArray().size());
      for (const auto& node : base_requests.AsArray()) {
        if (node.AsMap().at("type").AsString() == "Stop") {
          detail::Stop new_stop;
          new_stop.name = node.AsMap().at("name").AsString();
          new_stop.coordinates.lat = node.AsMap().at("latitude").AsDouble();
          new_stop.coordinates.lng = node.AsMap().at("longitude").AsDouble();
          for (const auto& [i, k] : node.AsMap().at("road_distances").AsMap()) {
            new_stop.distance.emplace_back(i, k.AsDouble());
          }
          stops.push_back(new_stop);
        } else if (node.AsMap().at("type").AsString() == "Bus") {
          temp_busses.push_back(node);
        } else {
          throw std::runtime_error("Invalid type");
        }
      }
      for (const auto& node : temp_busses) {
        detail::Bus new_bus;
        new_bus.number = node.AsMap().at("name").AsString();
        new_bus.is_roundtrip = node.AsMap().at("is_roundtrip").AsBool();
        new_bus.stops_names = ParseStopNames(node.AsMap().at("stops"));
        busses.push_back(new_bus);
      }
      base_requests_ = {stops, busses};
      return;
    } else {
      throw std::runtime_error("Bad base_request!");
    }
  } else {
    throw std::runtime_error("Bad base_request!");
  }
}

void JsonReader::LoadStatRequests() {
  auto stat_requests = doc_.GetRoot().AsMap().at("stat_requests");
  if (stat_requests != nullptr) {
    if (stat_requests.IsArray()) {
      for (const auto& node : stat_requests.AsArray()) {
        StatRequest new_stat_request;
        if (!node.IsMap()) {
          throw std::runtime_error("Bad stat request");
        }
        new_stat_request.id = node.AsMap().at("id").AsInt();
        new_stat_request.type = node.AsMap().at("type").AsString();
        new_stat_request.name = node.AsMap().at("name").AsString();
        stat_requests_.push_back(new_stat_request);
      }
      return;
    } else {
      throw std::runtime_error("Bad stat request");
    }
  } else {
    throw std::runtime_error("Bad stat request");
  }
}

std::pair<std::vector<detail::Stop>, std::vector<detail::Bus>>
JsonReader::GetBaseRequests() {
  return base_requests_;
}

[[maybe_unused]] std::vector<StatRequest> JsonReader::GetStatRequests() {
  return stat_requests_;
}

void JsonReader::ParseStatRequests(std::ostream& out) {
  std::vector<json::Node> info;
  for (const auto& request : stat_requests_) {
    if (request.type == "Stop") {
      auto stop = transport_catalogue_.FindStop(request.name);
      info.emplace_back(PrintStop(stop, request.id));
    } else if (request.type == "Bus") {
      auto bus = transport_catalogue_.GetBusInfo(request.name);
      info.emplace_back(PrintBus(bus, request.id));
    } else {
      throw std::runtime_error("Bad stat request");
    }
  }
  json::Document new_doc(info);
  json::Print(new_doc, out);
}

std::map<std::string, json::Node> JsonReader::PrintBus(const detail::Bus* bus, int id) {
  std::map<std::string, json::Node> dict;
  dict["request_id"] = json::Node(id);
  if(bus == nullptr){
    std::string error_message = "not found";
    dict["error_message"] = json::Node(error_message);
    return dict;
  }
  dict["curvature"] = json::Node(bus->curvature);
  dict["route_length"] = json::Node(bus->length_between_bus_stops);
  dict["stop_count"] = json::Node(int(bus->stops.size()));
  dict["unique_stop_count"] = json::Node(int(bus->unique_stops));
  return dict;
}

std::map<std::string, json::Node> JsonReader::PrintStop(detail::Stop* stop, int id) {
  std::map<std::string, json::Node> dict;
  dict["request_id"] = json::Node(id);
  if(stop == nullptr){
    std::string error_message = "not found";
    dict["error_message"] = json::Node(error_message);
    return dict;
  }
  std::set<std::string_view> temp = transport_catalogue_.GetBusesThroughStop(stop->name);
  std::vector<json::Node> busses;
  for(const auto& bus : temp){
    std::string a(bus);
    busses.emplace_back(a);
  }
  dict["buses"] = json::Node(busses);
  return dict;
}