#include "math_utils.h"
#include <stdexcept>

MathFunctionDispatcher& MathFunctionDispatcher::getInstance() {
    static MathFunctionDispatcher instance;
    return instance;
}

MathFunctionDispatcher::MathFunctionDispatcher() {
    // Initialize math function lookup table
    functions["sqr"] = [](double x) { return std::sqrt(x); };
    functions["sqrt"] = [](double x) { return std::sqrt(x); };
    functions["abs"] = [](double x) { return std::abs(x); };
    functions["sin"] = [](double x) { return std::sin(x); };
    functions["cos"] = [](double x) { return std::cos(x); };
    functions["tan"] = [](double x) { return std::tan(x); };
    functions["asin"] = [](double x) { return std::asin(x); };
    functions["acos"] = [](double x) { return std::acos(x); };
    functions["atan"] = [](double x) { return std::atan(x); };
    functions["log"] = safelog;
    functions["ln"] = safelog;  // ln is an alias for natural logarithm
    functions["log10"] = safelog10;
    functions["exp"] = [](double x) { return std::exp(x); };
    functions["floor"] = [](double x) { return std::floor(x); };
    functions["ceil"] = [](double x) { return std::ceil(x); };
    functions["round"] = [](double x) { return std::round(x); };
    functions["int"] = [](double x) { return static_cast<int>(x); };
}

double MathFunctionDispatcher::callFunction(const std::string& name, double arg) {
    auto it = functions.find(name);
    if (it != functions.end()) {
        return it->second(arg);
    }
    throw std::runtime_error("Unknown math function: " + name);
}

bool MathFunctionDispatcher::hasFunction(const std::string& name) const {
    return functions.find(name) != functions.end();
}

double MathFunctionDispatcher::safelog(double x) {
    if (x <= 0) {
        throw std::runtime_error("LOG requires a positive argument");
    }
    return std::log(x);
}

double MathFunctionDispatcher::safelog10(double x) {
    if (x <= 0) {
        throw std::runtime_error("LOG10 requires a positive argument");
    }
    return std::log10(x);
}