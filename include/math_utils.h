#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <cmath>

// Math function dispatcher for performance optimization
class MathFunctionDispatcher {
public:
    using MathFunction = std::function<double(double)>;
    
    static MathFunctionDispatcher& getInstance();
    double callFunction(const std::string& name, double arg);
    bool hasFunction(const std::string& name) const;
    
private:
    MathFunctionDispatcher();
    std::unordered_map<std::string, MathFunction> functions;
    
    // Helper functions for functions needing validation
    static double safelog(double x);
    static double safelog10(double x);
};