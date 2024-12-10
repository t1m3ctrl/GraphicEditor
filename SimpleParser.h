#include <QString>
#include <QStack>
#include <QMap>
#include <cmath>
#include <QVariant>
#include <iostream>

class SimpleParser {
public:
    static double evaluate(const QString &expression, double x) {
        // Преобразуем инфиксное выражение в постфиксное
        QStringList tokens = tokenize(expression);
        QStringList output = infixToPostfix(tokens);
        return evaluatePostfix(output, x);
    }

private:
    static QStringList tokenize(const QString &expr) {
        QStringList tokens;
        QString currentToken;
        bool expectNumberOrUnaryMinus = true;  // Ожидаем число или унарный минус в начале или после оператора
        for (int i = 0; i < expr.size(); ++i) {
            QChar ch = expr[i];
            if (ch.isDigit() || ch == '.' || ch == 'x') {
                currentToken += ch;
                expectNumberOrUnaryMinus = false;  // Следующий символ не может быть унарным минусом
            } else if (ch.isSpace()) {
                if (!currentToken.isEmpty()) {
                    tokens.append(currentToken);
                    currentToken.clear();
                }
            } else if (QString("+-*/^()").contains(ch)) {
                if (!currentToken.isEmpty()) {
                    tokens.append(currentToken);
                    currentToken.clear();
                }
                // Унарный минус: если ожидается число или переменная, то это унарный минус
                if (ch == '-' && expectNumberOrUnaryMinus) {
                    currentToken += ch;  // Считаем, что это часть числа
                } else {
                    tokens.append(QString(ch));
                }
                expectNumberOrUnaryMinus = true;  // После оператора ждем число
            }
        }
        if (!currentToken.isEmpty()) {
            tokens.append(currentToken);
        }
        return tokens;
    }


    static QStringList infixToPostfix(const QStringList &tokens) {
        QStack<QString> operators;
        QStringList output;

        QMap<QString, int> precedence = {{"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}, {"^", 3}};

        for (const QString &token : tokens) {
            if (token == "(") {
                operators.push(token);
            } else if (token == ")") {
                while (!operators.isEmpty() && operators.top() != "(") {
                    output.append(operators.pop());
                }
                operators.pop();  // Убираем '('
            } else if (precedence.contains(token)) {
                while (!operators.isEmpty() && precedence[operators.top()] >= precedence[token]) {
                    output.append(operators.pop());
                }
                operators.push(token);
            } else {  // Число или переменная x
                output.append(token);
            }
        }

        while (!operators.isEmpty()) {
            output.append(operators.pop());
        }

        return output;
    }

    static double evaluatePostfix(const QStringList &tokens, double x) {
        QStack<double> stack;

        for (const QString &token : tokens) {
            if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
                double b = stack.pop();
                double a = stack.pop();
                if (token == "+") {
                    stack.push(a + b);
                } else if (token == "-") {
                    stack.push(a - b);
                } else if (token == "*") {
                    stack.push(a * b);
                } else if (token == "/") {
                    stack.push(a / b);
                } else if (token == "^") {
                    stack.push(std::pow(a, b));
                }
            } else if (token == "sin") {
                double value = stack.pop();
                stack.push(std::sin(value));
            } else if (token == "cos") {
                double value = stack.pop();
                stack.push(std::cos(value));
            } else if (token == "exp") {
                double value = stack.pop();
                stack.push(std::exp(value));
            } else if (token == "x") {
                stack.push(x);
            } else {
                stack.push(token.toDouble());
            }
        }

        return stack.pop();
    }

};
