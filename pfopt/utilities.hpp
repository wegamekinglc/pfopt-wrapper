#ifndef pfopt_utilities_hpp
#define pfopt_utilities_hpp

#include "types.hpp"

namespace pfopt {

    namespace io {
        MatrixXd read_csv(const std::string& filePath);
        void write_csv(std::string& filePath, MatrixXd matrix);
    }
}

#endif