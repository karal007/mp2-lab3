#include "translator.h"
#include <cctype>
#include <cmath>
#include <sstream>

Translator::Translator(const std::string& expression) : expression_(expression) {
    tokenize();
    toPostfix();
}

bool Translator::isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

int Translator::getPriority(char op) {
    switch (op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        case '^':
            return 3;
        case '~': // Унарный минус
            return 4;
        default:
            return 0;
    }
}

void Translator::tokenize() {
    size_t i = 0;
    const size_t n = expression_.size();
    bool expect_operand = true; // Для распознавания унарного минуса

    while (i < n) {
        char c = expression_[i];

        if (std::isspace(static_cast<unsigned char>(c))) {
            ++i;
            continue;
        }

        // Число (целое или с плавающей точкой)
        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            size_t start = i;
            bool has_dot = false;
            while (i < n && (std::isdigit(static_cast<unsigned char>(expression_[i])) || expression_[i] == '.')) {
                if (expression_[i] == '.') {
                    if (has_dot) {
                        throw std::invalid_argument("Invalid number format: multiple dots");
                    }
                    has_dot = true;
                }
                ++i;
            }
            std::string num_str = expression_.substr(start, i - start);
            if (num_str == ".") {
                throw std::invalid_argument("Invalid standalone dot");
            }
            infix_tokens_.push({TokenType::Number, num_str, std::stod(num_str)});
            expect_operand = false;
            continue;
        }

        // Открывающая скобка
        if (c == '(') {
            infix_tokens_.push({TokenType::LeftParen, "(", 0.0});
            expect_operand = true;
            ++i;
            continue;
        }

        // Закрывающая скобка
        if (c == ')') {
            if (expect_operand) {
                throw std::invalid_argument("Syntax error: empty parentheses or missing operand before ')'");
            }
            infix_tokens_.push({TokenType::RightParen, ")", 0.0});
            expect_operand = false;
            ++i;
            continue;
        }

        // Операторы
        if (isOperator(c)) {
            if (expect_operand) {
                if (c == '-') {
                    infix_tokens_.push({TokenType::Operator, "~", 0.0}); // Унарный минус
                    ++i;
                    continue;
                } else if (c == '+') {
                    // Унарный плюс можно просто пропустить
                    ++i;
                    continue;
                } else {
                    throw std::invalid_argument("Syntax error: unexpected binary operator");
                }
            }
            infix_tokens_.push({TokenType::Operator, std::string(1, c), 0.0});
            expect_operand = true;
            ++i;
            continue;
        }

        throw std::invalid_argument(std::string("Unknown character in expression: ") + c);
    }

    if (expect_operand) {
        throw std::invalid_argument("Syntax error: expression cannot end with an operator or be empty");
    }
}

void Translator::toPostfix() {
    Stack<Token> op_stack;
    Queue<Token> tokens_copy = infix_tokens_;

    while (!tokens_copy.empty()) {
        Token token = tokens_copy.front();
        tokens_copy.pop();

        if (token.type == TokenType::Number) {
            postfix_tokens_.push(token);
        } else if (token.type == TokenType::LeftParen) {
            op_stack.push(token);
        } else if (token.type == TokenType::RightParen) {
            bool found_left_paren = false;
            while (!op_stack.empty()) {
                if (op_stack.top().type == TokenType::LeftParen) {
                    op_stack.pop();
                    found_left_paren = true;
                    break;
                }
                postfix_tokens_.push(op_stack.top());
                op_stack.pop();
            }
            if (!found_left_paren) {
                throw std::invalid_argument("Mismatched parentheses: extra ')'");
            }
        } else if (token.type == TokenType::Operator) {
            char curr_op = token.text[0];
            int curr_prio = getPriority(curr_op);

            while (!op_stack.empty() && op_stack.top().type == TokenType::Operator) {
                char top_op = op_stack.top().text[0];
                int top_prio = getPriority(top_op);

                // Правоассоциативные операторы: '^' и унарный '~'
                bool is_right_assoc = (curr_op == '^' || curr_op == '~');
                if ((!is_right_assoc && curr_prio <= top_prio) ||
                    (is_right_assoc && curr_prio < top_prio)) {
                    postfix_tokens_.push(op_stack.top());
                    op_stack.pop();
                } else {
                    break;
                }
            }
            op_stack.push(token);
        }
    }

    while (!op_stack.empty()) {
        if (op_stack.top().type == TokenType::LeftParen) {
            throw std::invalid_argument("Mismatched parentheses: unclosed '('");
        }
        postfix_tokens_.push(op_stack.top());
        op_stack.pop();
    }
}

std::string Translator::getPostfixString() const {
    Queue<Token> copy = postfix_tokens_;
    std::ostringstream oss;
    bool first = true;
    while (!copy.empty()) {
        if (!first) {
            oss << " ";
        }
        oss << copy.front().text;
        copy.pop();
        first = false;
    }
    return oss.str();
}

double Translator::calculate() {
    Stack<double> eval_stack;
    Queue<Token> copy = postfix_tokens_;

    while (!copy.empty()) {
        Token token = copy.front();
        copy.pop();

        if (token.type == TokenType::Number) {
            eval_stack.push(token.value);
        } else if (token.type == TokenType::Operator) {
            char op = token.text[0];
            if (op == '~') {
                if (eval_stack.empty()) {
                    throw std::logic_error("Evaluation error: missing operand for unary minus");
                }
                double val = eval_stack.top();
                eval_stack.pop();
                eval_stack.push(-val);
            } else {
                if (eval_stack.size() < 2) {
                    throw std::logic_error("Evaluation error: insufficient operands");
                }
                double rhs = eval_stack.top();
                eval_stack.pop();
                double lhs = eval_stack.top();
                eval_stack.pop();

                switch (op) {
                    case '+': eval_stack.push(lhs + rhs); break;
                    case '-': eval_stack.push(lhs - rhs); break;
                    case '*': eval_stack.push(lhs * rhs); break;
                    case '/':
                        if (std::abs(rhs) < 1e-12) {
                            throw std::runtime_error("Division by zero");
                        }
                        eval_stack.push(lhs / rhs);
                        break;
                    case '^': eval_stack.push(std::pow(lhs, rhs)); break;
                    default: throw std::logic_error("Unknown operator");
                }
            }
        }
    }

    if (eval_stack.size() != 1) {
        throw std::logic_error("Evaluation error: malformed expression");
    }

    return eval_stack.top();
}