#include "./database.hpp"

#include "./simple_parser.hpp"

using namespace std;

Database::Database(istream& input)
    : outline_lower_left_(0, 0), outline_upper_right_(0, 0) {
  Parse(input);
}

void Database::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Database:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ')
     << "outline_lower_left_:" << endl;
  ++indent_level;
  outline_lower_left_.Print(os, indent_level);
  --indent_level;
  os << string(indent_level * indent_size, ' ')
     << "outline_upper_right_:" << endl;
  ++indent_level;
  outline_upper_right_.Print(os, indent_level);
  --indent_level;
  os << string(indent_level * indent_size, ' ') << "pins_:" << endl;
  ++indent_level;
  for (const Pin& pin : pins_) {
    pin.Print(os, indent_level);
  }
  --indent_level;
}

const Point& Database::outline_lower_left() const {
  return outline_lower_left_;
}

const Point& Database::outline_upper_right() const {
  return outline_upper_right_;
}

int Database::num_pins() const { return pins_.size(); }

const Pin& Database::pin(int id) const { return pins_.at(id); }

// Private

void Database::Parse(istream& input) {
  simple_parser::Parser parser(input, "=,()");
  int num_pins = 0;
  int nth_pin = 0;
  parser.Parse([&](const simple_parser::Tokens& tokens) -> bool {
    if (!tokens.empty()) {
      string first_word = tokens[0];
      if (first_word == "Boundary") {
        outline_lower_left_ = Point(stoi(tokens[1]), stoi(tokens[2]));
        outline_upper_right_ = Point(stoi(tokens[3]), stoi(tokens[4]));
      } else if (first_word == "NumPins") {
        num_pins = stoi(tokens[1]);
        pins_.reserve(num_pins);
      } else if (first_word == "PIN") {
        if (nth_pin < num_pins) {
          ++nth_pin;
          pins_.push_back(Pin(tokens[1], stoi(tokens[2]), stoi(tokens[3])));
        }
      }
    }
    return true;
  });
}
