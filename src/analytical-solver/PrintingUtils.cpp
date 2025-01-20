#include "PrintingUtils.h"

#include <iomanip>

namespace DungeonGenerator {
namespace AnalyticalSolver {
namespace PrintingUtils {

PrecisionSetter::PrecisionSetter(std::ostream& ostream)
      : ostream_(ostream)
{
    oldPrecision_ = std::cerr.precision();
    ostream_ << std::setprecision(PRINT_PRECISION);
}

PrecisionSetter::~PrecisionSetter()
{
    ostream_ << std::setprecision(oldPrecision_);
}

std::string arrayToString(const double* array, size_t size)
{
    std::ostringstream ss;
    PrecisionSetter precisionSetter(ss);

    ss << "(";
    for (size_t i = 0; i < size; ++i) {
        ss << array[i];
        if (i < size - 1) {
            ss << ", ";
        }
    }
    ss << ")";
    return ss.str();
}

std::string matrixToString(const double* matrix, const size_t rows, const size_t cols, const std::string& rowPadding)
{
    std::ostringstream ss;
    PrecisionSetter precisionSetter(ss);

    for (size_t i = 0; i < rows; ++i) {
        if (i > 0) {
            ss << rowPadding;
        }
        for (size_t j = 0; j < cols; ++j) {
            ss << matrix[i * cols + j];
            if (j < cols - 1) {
                ss << ", ";
            }
        }
        if (i < rows - 1) {
            ss << "\n";
        }
    }
    return ss.str();
}

}  // namespace PrintingUtils
}  // namespace AnalyticalSolver
}  // namespace DungeonGenerator
