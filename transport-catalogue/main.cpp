#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

int main() {
  transport::TransportCatalogue new_cat;
  MapRenderer map_renderer;
  RequestHandler requestHandler(new_cat, map_renderer);
  json::Document doc = json::Load(std::cin);
  JsonReader json_doc(new_cat, requestHandler, doc);
  json_doc.ParseStatRequests(std::cout);
}
