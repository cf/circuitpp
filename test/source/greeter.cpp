#include <doctest/doctest.h>
#include <greeter/greeter.h>

#include <string>

TEST_CASE("CircuitPP") {
  using namespace greeter;

  CircuitPP greeter("Tests");

  CHECK(greeter.greet(LanguageCode::EN) == "Hello, Tests!");
  CHECK(greeter.greet(LanguageCode::DE) == "Hallo Tests!");
  CHECK(greeter.greet(LanguageCode::ES) == "¡Hola Tests!");
  CHECK(greeter.greet(LanguageCode::FR) == "Bonjour Tests!");
}
