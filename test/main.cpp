#include "json_reader.h"
#include "request_handler.h"

int main() {
  /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON
   */
  transport::TransportCatalogue new_cat;
  json::Document doc = json::Load(std::cin);
  JsonReader json_doc(doc, new_cat);
  RequestHandler requestHandler(new_cat, json_doc.GetBaseRequests());
  json_doc.ParseStatRequests(std::cout);
}
