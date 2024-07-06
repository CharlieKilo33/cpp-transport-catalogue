#include "json_reader.h"

#include <sstream>

JsonReader::JsonReader(transport::TransportCatalogue& transport_catalogue,
                       RequestHandler& request_handler, json::Document& doc)
    : transport_catalogue_(transport_catalogue),
      request_handler_(request_handler),
      doc_(doc) {
  LoadBaseRequests();
  LoadStatRequests();
  LoadRenderSettings();
}

std::vector<std::string> JsonReader::ParseStopNames(const json::Node& array) {
  if (!array.IsArray()) {
    std::cerr << "Not array";
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
  try {
    doc_.GetRoot().AsMap().at("base_requests");
  } catch (const std::out_of_range& e) {
    std::cerr << "Error: " << e.what() << ". No 'base_requests' in JSON." << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Unknown error: " << e.what() << std::endl;
  }
  auto base_requests = doc_.GetRoot().AsMap().at("base_requests");
  if (base_requests != nullptr) {
    if (base_requests.IsArray()) {
      std::vector<json::Node> temp_buses;
      std::vector<detail::Stop> stops;
      std::vector<detail::Bus> buses;
      stops.reserve(base_requests.AsArray().size());
      buses.reserve(base_requests.AsArray().size());
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
          temp_buses.push_back(node);
        } else {
          std::cerr << "Invalid type";
          throw std::runtime_error("Invalid type");
        }
      }
      for (const auto& node : temp_buses) {
        detail::Bus new_bus;
        new_bus.number = node.AsMap().at("name").AsString();
        new_bus.is_roundtrip = node.AsMap().at("is_roundtrip").AsBool();
        new_bus.stops_names = ParseStopNames(node.AsMap().at("stops"));
        buses.push_back(new_bus);
      }
      request_handler_.AddRequests({stops, buses});
      return;
    } else {
      std::cerr << "Bad stat request";
      throw std::runtime_error("Bad base_request!");
    }
  } else {
    std::cerr << "Bad stat request";
    throw std::runtime_error("Bad base_request!");
  }
}

void JsonReader::LoadStatRequests() {
  try {
    doc_.GetRoot().AsMap().at("stat_requests");
  } catch (const std::out_of_range& e) {
    std::cerr << "Error: " << e.what() << ". No 'stat_requests' in JSON." << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "Unknown error: " << e.what() << std::endl;
  }
  auto stat_requests = doc_.GetRoot().AsMap().at("stat_requests");
  if (stat_requests != nullptr) {
    if (stat_requests.IsArray()) {
      for (const auto& node : stat_requests.AsArray()) {
        StatRequest new_stat_request;
        if (!node.IsMap()) {
          std::cerr << "Bad stat request";
          throw std::runtime_error("Bad stat request");
        }
        new_stat_request.id = node.AsMap().at("id").AsInt();
        new_stat_request.type = node.AsMap().at("type").AsString();
        if(new_stat_request.type != "Map"){
          new_stat_request.name = node.AsMap().at("name").AsString();
        }
        stat_requests_.push_back(new_stat_request);
      }
      return;
    } else {
      std::cerr << "Bad stat request";
      throw std::runtime_error("Bad stat request");
    }
  } else {
    std::cerr << "Bad stat request";
    throw std::runtime_error("Bad stat request");
  }
}

svg::Color ParseColor(const json::Node& node) {
  svg::Color new_color;
  if (node.IsString()) {
    new_color = node.AsString();
  } else if (node.IsArray() && node.AsArray().size() == 3) {
    svg::Rgb rgb;
    rgb.red = uint8_t(node.AsArray()[0].AsDouble());
    rgb.green = uint8_t(node.AsArray()[1].AsDouble());
    rgb.blue = uint8_t(node.AsArray()[2].AsDouble());
    new_color = rgb;
  } else if (node.IsArray() && node.AsArray().size() == 4) {
    svg::Rgba rgba;
    rgba.red = uint8_t(node.AsArray()[0].AsDouble());
    rgba.green = uint8_t(node.AsArray()[1].AsDouble());
    rgba.blue = uint8_t(node.AsArray()[2].AsDouble());
    rgba.opacity = node.AsArray()[3].AsDouble();
    new_color = rgba;
  }
  return new_color;
}

void JsonReader::LoadRenderSettings() {
  try {
    doc_.GetRoot().AsMap().at("render_settings");
  } catch (const std::out_of_range& e) {
    std::cerr << "Error: " << e.what() << ". No 'render_settings' in JSON." << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Unknown error: " << e.what() << std::endl;
  }
  auto render_settings = doc_.GetRoot().AsMap().at("render_settings");
  if (render_settings.IsMap()) {
    RenderSettings new_render_settings;
    new_render_settings.width = render_settings.AsMap().at("width").AsDouble();
    new_render_settings.height =
        render_settings.AsMap().at("height").AsDouble();
    new_render_settings.bus_label_font_size =
        render_settings.AsMap().at("bus_label_font_size").AsInt();
    new_render_settings.bus_label_offset = {
        render_settings.AsMap().at("bus_label_offset").AsArray()[0].AsDouble(),
        render_settings.AsMap().at("bus_label_offset").AsArray()[1].AsDouble()};
    new_render_settings.line_width =
        render_settings.AsMap().at("line_width").AsDouble();
    new_render_settings.padding =
        render_settings.AsMap().at("padding").AsDouble();
    new_render_settings.stop_radius =
        render_settings.AsMap().at("stop_radius").AsDouble();
    new_render_settings.stop_label_font_size =
        render_settings.AsMap().at("stop_label_font_size").AsInt();
    new_render_settings.stop_label_offset = {
        render_settings.AsMap().at("stop_label_offset").AsArray()[0].AsDouble(),
        render_settings.AsMap()
            .at("stop_label_offset")
            .AsArray()[1]
            .AsDouble()};
    new_render_settings.underlayer_width =
        render_settings.AsMap().at("underlayer_width").AsDouble();
    new_render_settings.underlayer_color =
        ParseColor(render_settings.AsMap().at("underlayer_color"));
    std::vector<svg::Color> temp_colors;
    for (const auto& node :
         render_settings.AsMap().at("color_palette").AsArray()) {
      temp_colors.push_back(ParseColor(node));
    }
    new_render_settings.color_palette = temp_colors;
    request_handler_.AddRenderContext(new_render_settings);
  }
}

void JsonReader::ParseStatRequests(std::ostream& out) {
  std::vector<json::Node> info;
  for (const auto& request : stat_requests_) {
    if (request.type == "Stop") {
      auto stop = transport_catalogue_.FindStop(request.name);
      info.emplace_back(GetMapStops(stop, request.id));
    } else if (request.type == "Bus") {
      auto bus = transport_catalogue_.GetBusInfo(request.name);
      info.emplace_back(GetMapBuses(bus, request.id));
    } else if (request.type == "Map"){
      info.emplace_back(GetMapMap(request.id));
    }
  }
  if(info.empty()){
    return;
  }
  json::Document new_doc(info);
  json::Print(new_doc, out);
}

std::map<std::string, json::Node> JsonReader::GetMapBuses(
    const detail::Bus* bus, int id) {
  std::map<std::string, json::Node> dict;
  dict["request_id"] = json::Node(id);
  if (bus == nullptr) {
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

std::map<std::string, json::Node> JsonReader::GetMapStops(detail::Stop* stop,
                                                          int id) {
  std::map<std::string, json::Node> dict;
  dict["request_id"] = json::Node(id);
  if (stop == nullptr) {
    std::string error_message = "not found";
    dict["error_message"] = json::Node(error_message);
    return dict;
  }
  std::set<std::string_view> temp =
      transport_catalogue_.GetBusesThroughStop(stop->name);
  std::vector<json::Node> buses;
  for (const auto& bus : temp) {
    std::string a(bus);
    buses.emplace_back(a);
  }
  dict["buses"] = json::Node(buses);
  return dict;
}

std::map<std::string, json::Node> JsonReader::GetMapMap(int id) {
  std::map<std::string, json::Node> dict;
  dict["request_id"] = json::Node(id);
  std::ostringstream map;
  request_handler_.RenderMap().Render(map);
  dict["map"] = json::Node(map.str());
  return dict;
}