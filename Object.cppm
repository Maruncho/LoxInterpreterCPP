
export module Object;

import <variant>;
import <string>;
import <iostream>;

export class Object {

	std::variant<double, bool, std::string, std::monostate> val;
	int line;

public:
	Object(double value) : val{ value } { }
	Object(bool value) : val{ value } { }
	Object(std::string value) : val{ value } { }
	Object() : val{ std::monostate() } {}

	const double getDouble() const { return std::get<double>(val); }
	const bool getBool() const { return std::get<bool>(val); }
	const std::string getString() const { return std::get<std::string>(val); }
	
	bool isDouble() const { return std::holds_alternative<double>(val); }
	bool isBool() const { return std::holds_alternative<bool>(val); }
	bool isString() const { return std::holds_alternative<std::string>(val); }
	bool isNil() const { return std::holds_alternative<std::monostate>(val); }

	bool equals(const Object other) const {
		return val == other.val;
	}
};

export std::ostream& operator<<(std::ostream& os, const Object& obj) {
	if (obj.isDouble()) return os << obj.getDouble();
	if (obj.isBool()) return os << (obj.getBool() ? "true" : "false");
	if (obj.isString()) return os << obj.getString();
	if (obj.isNil()) return os << "nil";
	else return os; //unreachable
}
