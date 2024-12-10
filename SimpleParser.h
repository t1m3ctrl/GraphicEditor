#include <tinyexpr.h>
#include <QVector>
#include <cmath>
#include <QString>
#include <QDebug>

class SimpleParser {
public:
    static double evaluate(const QString &expression, double x) {
        std::string expr = expression.toStdString();

        const char *varName = "x";

        te_variable vars[] = {
            {varName, &x, 0, nullptr}
        };

        te_expr *e = te_compile(expr.c_str(), vars, 1, nullptr);

        if (!e) {
            // qDebug() << "Error compiling expression:" << expression;
            return std::numeric_limits<double>::quiet_NaN();
        }

        double result = te_eval(e);

        te_free(e);

        if (std::isnan(result) || std::isinf(result)) {
            return std::numeric_limits<double>::quiet_NaN();
        }

        return result;
    }
};
