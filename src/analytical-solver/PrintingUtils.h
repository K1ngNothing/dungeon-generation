#pragma once

#include <iostream>

namespace DungeonGenerator {
namespace AnalyticalSolver {
namespace PrintingUtils {

constexpr size_t ARRAY_PRINT_LIMIT = 8;  // How many variables are we willing to print
constexpr size_t PRINT_PRECISION = 8;    // Precision for printing double values

// Helper class for setting output precision
class PrecisionSetter {
public:
    PrecisionSetter(std::ostream& ostream);
    ~PrecisionSetter();

private:
    std::ostream& ostream_;
    std::streamsize oldPrecision_;
};

std::string arrayToString(const double* array, size_t size);
std::string matrixToString(const double* matrix, const size_t rows, const size_t cols, const std::string& rowPadding);

}  // namespace PrintingUtils
}  // namespace AnalyticalSolver
}  // namespace DungeonGenerator
