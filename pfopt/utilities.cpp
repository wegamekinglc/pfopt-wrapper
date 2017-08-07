#include "utilities.hpp"
#include "minicsv.hpp"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace pfopt {

    namespace io {
        MatrixXd read_csv(const std::string& filePath) {

            std::vector<std::vector<double>> table;

            mini::csv::ifstream is(filePath);

            if (is.is_open()) {
                while (is.read_line()) {
                    std::string field;
                    is >> field;
                    std::vector<double> row;
                    while (field != "") {
                        row.push_back(boost::lexical_cast<double>(field));
                        is >> field;
                    }
                    table.push_back(row);
                }
            } else {
                throw std::runtime_error(str(boost::format("file %s is not found") % filePath));
            }

            MatrixXd res(table.size(), table[0].size());

            for (size_t i = 0; i != res.rows(); ++i)
                for (size_t j = 0; j != res.cols(); ++j)
                    res(i, j) = table[i][j];

            return res;
        }

        void write_csv(std::string& filePath, MatrixXd matrix) {

            std::ofstream out(filePath);
            int row = matrix.rows();
            int col = matrix.cols();
            for (int i = 0; i < row; i++) {
                for (int j = 0; j < col; j++)
                    out << matrix(i, j) << ',';
                out << '\n';
            }
        }
    }
}