#pragma once
#include <list>

#include "domain.h"
#include "geo.h"
#include "json_reader.h"
#include "transport_catalogue.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего
 * логику, которую не хотелось бы помещать ни в transport_catalogue, ни в json
 * reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика
 * запросов. Вы можете реализовать обработку запросов способом, который удобнее
 * вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON
// reader-а с другими подсистемами приложения. См. паттерн проектирования Фасад:
// https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)

class RequestHandler {
 public:
  RequestHandler(transport::TransportCatalogue& transport_catalogue,
                 std::pair<std::vector<detail::Stop>, std::vector<detail::Bus>>
                     base_requests);

 private:
  void ParseBusses(std::vector<detail::Bus>& busses);
  transport::TransportCatalogue& transport_catalogue_;
  std::pair<std::vector<detail::Stop>, std::vector<detail::Bus>> base_requests_;
};