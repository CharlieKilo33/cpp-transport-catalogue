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
    doc_.GetRoot().AsDict().at("base_requests");
  } catch (const std::out_of_range& e) {
    std::cerr << "Error: " << e.what() << ". No 'base_requests' in JSON."
              << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Unknown error: " << e.what() << std::endl;
  }
  auto base_requests = doc_.GetRoot().AsDict().at("base_requests");
  if (base_requests != nullptr) {
    if (base_requests.IsArray()) {
      std::vector<json::Node> temp_buses;
      std::vector<detail::Stop> stops;
      std::vector<detail::Bus> buses;
      stops.reserve(base_requests.AsArray().size());
      buses.reserve(base_requests.AsArray().size());
      for (const auto& node : base_requests.AsArray()) {
        if (node.AsDict().at("type").AsString() == "Stop") {
          detail::Stop new_stop;
          new_stop.name = node.AsDict().at("name").AsString();
          new_stop.coordinates.lat = node.AsDict().at("latitude").AsDouble();
          new_stop.coordinates.lng = node.AsDict().at("longitude").AsDouble();
          for (const auto& [i, k] :
               node.AsDict().at("road_distances").AsDict()) {
            new_stop.distance.emplace_back(i, k.AsDouble());
          }
          stops.push_back(new_stop);
        } else if (node.AsDict().at("type").AsString() == "Bus") {
          temp_buses.push_back(node);
        } else {
          std::cerr << "Invalid type";
          throw std::runtime_error("Invalid type");
        }
      }
      for (const auto& node : temp_buses) {
        detail::Bus new_bus;
        new_bus.number = node.AsDict().at("name").AsString();
        new_bus.is_roundtrip = node.AsDict().at("is_roundtrip").AsBool();
        new_bus.stops_names = ParseStopNames(node.AsDict().at("stops"));
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
    doc_.GetRoot().AsDict().at("stat_requests");
  } catch (const std::out_of_range& e) {
    std::cerr << "Error: " << e.what() << ". No 'stat_requests' in JSON."
              << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "Unknown error: " << e.what() << std::endl;
  }
  auto stat_requests = doc_.GetRoot().AsDict().at("stat_requests");
  if (stat_requests != nullptr) {
    if (stat_requests.IsArray()) {
      for (const auto& node : stat_requests.AsArray()) {
        StatRequest new_stat_request;
        if (!node.IsDict()) {
          std::cerr << "Bad stat request";
          throw std::runtime_error("Bad stat request");
        }
        new_stat_request.id = node.AsDict().at("id").AsInt();
        new_stat_request.type = node.AsDict().at("type").AsString();
        if (new_stat_request.type != "Map") {
          new_stat_request.name = node.AsDict().at("name").AsString();
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
    doc_.GetRoot().AsDict().at("render_settings");
  } catch (const std::out_of_range& e) {
    std::cerr << "Error: " << e.what() << ". No 'render_settings' in JSON."
              << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Unknown error: " << e.what() << std::endl;
  }
  auto render_settings = doc_.GetRoot().AsDict().at("render_settings");
  if (render_settings.IsDict()) {
    RenderSettings new_render_settings;
    new_render_settings.width = render_settings.AsDict().at("width").AsDouble();
    new_render_settings.height =
        render_settings.AsDict().at("height").AsDouble();
    new_render_settings.bus_label_font_size =
        render_settings.AsDict().at("bus_label_font_size").AsInt();
    new_render_settings.bus_label_offset = {
        render_settings.AsDict().at("bus_label_offset").AsArray()[0].AsDouble(),
        render_settings.AsDict()
            .at("bus_label_offset")
            .AsArray()[1]
            .AsDouble()};
    new_render_settings.line_width =
        render_settings.AsDict().at("line_width").AsDouble();
    new_render_settings.padding =
        render_settings.AsDict().at("padding").AsDouble();
    new_render_settings.stop_radius =
        render_settings.AsDict().at("stop_radius").AsDouble();
    new_render_settings.stop_label_font_size =
        render_settings.AsDict().at("stop_label_font_size").AsInt();
    new_render_settings.stop_label_offset = {render_settings.AsDict()
                                                 .at("stop_label_offset")
                                                 .AsArray()[0]
                                                 .AsDouble(),
                                             render_settings.AsDict()
                                                 .at("stop_label_offset")
                                                 .AsArray()[1]
                                                 .AsDouble()};
    new_render_settings.underlayer_width =
        render_settings.AsDict().at("underlayer_width").AsDouble();
    new_render_settings.underlayer_color =
        ParseColor(render_settings.AsDict().at("underlayer_color"));
    std::vector<svg::Color> temp_colors;
    for (const auto& node :
         render_settings.AsDict().at("color_palette").AsArray()) {
      temp_colors.push_back(ParseColor(node));
    }
    new_render_settings.color_palette = temp_colors;
    request_handler_.AddRenderContext(new_render_settings);
  }
}

void JsonReader::ParseStatRequests(std::ostream& out) {
  json::Builder builder;
  builder.StartArray();
  for (const auto& request : stat_requests_) {
    if (request.type == "Stop") {
      auto stop = transport_catalogue_.FindStop(request.name);
      GetMapStops(stop, request.id, builder);
    } else if (request.type == "Bus") {
      auto bus = transport_catalogue_.GetBusInfo(request.name);
      GetMapBuses(bus, request.id, builder);
    } else if (request.type == "Map") {
      GetMapMap(request.id, builder);
    }
  }
  if (builder.Empty()) {
    return;
  }
  builder.EndArray();
  json::Print(json::Document{builder.Build()}, out);
}

void JsonReader::GetMapBuses(
    const detail::Bus* bus, int id, json::Builder& builder) {
  builder.StartDict().Key("request_id").Value(id);
  if (bus == nullptr) {
    builder.Key("error_message").Value("not found");
    builder.EndDict();
    return;
  }
  builder.Key("curvature").Value(bus->curvature);
  builder.Key("route_length").Value(bus->length_between_bus_stops);
  builder.Key("stop_count").Value(int(bus->stops.size()));
  builder.Key("unique_stop_count").Value(int(bus->unique_stops));
  builder.EndDict();
}

void JsonReader::GetMapStops(
    detail::Stop* stop, int id, json::Builder& builder) {
  builder.StartDict().Key("request_id").Value(id);
  if (stop == nullptr) {
    builder.Key("error_message").Value("not found");
    builder.EndDict();
    return;
  }
  std::set<std::string_view> temp =
      transport_catalogue_.GetBusesThroughStop(stop->name);
  std::vector<json::Node> buses;
  for (const auto& bus : temp) {
    std::string a(bus);
    buses.emplace_back(a);
  }
  builder.Key("buses").Value(buses);
  builder.EndDict();
}

void JsonReader::GetMapMap(int id, json::Builder& builder) {
  builder.StartDict().Key("request_id").Value(id);
  std::ostringstream map;
  request_handler_.RenderMap().Render(map);
  builder.Key("map").Value(map.str());
  builder.EndDict();
}