#pragma once
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>

class JsonValue {
public:
    enum class Type {
        Null,
        String,
        Number,
        Boolean,
        Array,
        Object
    };

    JsonValue() : type_(Type::Null) {}
    JsonValue(const std::string& value) : type_(Type::String), string_value_(value) {}
    JsonValue(int value) : type_(Type::Number), number_value_(value) {}
    JsonValue(bool value) : type_(Type::Boolean), bool_value_(value) {}
    JsonValue(const std::vector<JsonValue>& value) : type_(Type::Array), array_value_(value) {}
    JsonValue(const std::map<std::string, JsonValue>& value) : type_(Type::Object), object_value_(value) {}

    std::string toString(int indent = 0) const {
        std::stringstream ss;
        std::string indent_str(indent * 4, ' ');

        switch (type_) {
            case Type::Null:
                ss << "null";
                break;
            case Type::String:
                ss << "\"" << string_value_ << "\"";
                break;
            case Type::Number:
                ss << number_value_;
                break;
            case Type::Boolean:
                ss << (bool_value_ ? "true" : "false");
                break;
            case Type::Array:
                ss << "[\n";
                for (size_t i = 0; i < array_value_.size(); ++i) {
                    ss << indent_str << "    " << array_value_[i].toString(indent + 1);
                    if (i < array_value_.size() - 1) ss << ",";
                    ss << "\n";
                }
                ss << indent_str << "]";
                break;
            case Type::Object:
                ss << "{\n";
                size_t i = 0;
                for (const auto& [key, value] : object_value_) {
                    ss << indent_str << "    \"" << key << "\": " << value.toString(indent + 1);
                    if (i < object_value_.size() - 1) ss << ",";
                    ss << "\n";
                    ++i;
                }
                ss << indent_str << "}";
                break;
        }
        return ss.str();
    }

    void push_back(const JsonValue& value) {
        if (type_ == Type::Array) {
            array_value_.push_back(value);
        }
    }

    JsonValue& operator[](const std::string& key) {
        if (type_ != Type::Object) {
            type_ = Type::Object;
            object_value_.clear();
        }
        return object_value_[key];
    }

    JsonValue& operator=(const std::string& value) {
        type_ = Type::String;
        string_value_ = value;
        return *this;
    }

    JsonValue& operator=(int value) {
        type_ = Type::Number;
        number_value_ = value;
        return *this;
    }

    JsonValue& operator=(bool value) {
        type_ = Type::Boolean;
        bool_value_ = value;
        return *this;
    }

private:
    Type type_;
    std::string string_value_;
    int number_value_ = 0;
    bool bool_value_ = false;
    std::vector<JsonValue> array_value_;
    std::map<std::string, JsonValue> object_value_;
};

class Json {
public:
    static JsonValue array() {
        return JsonValue(std::vector<JsonValue>());
    }

    static JsonValue object() {
        return JsonValue(std::map<std::string, JsonValue>());
    }
}; 