#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>

// ==========================================
// 1. Динамический массив (Vector)
// ==========================================
template <typename T>
class Vector {
private:
    T* data_;
    size_t size_;
    size_t capacity_;

    void reallocate(size_t new_capacity) {
        T* new_data = new T[new_capacity];
        for (size_t i = 0; i < size_; ++i) {
            new_data[i] = std::move(data_[i]);
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }

public:
    Vector() : data_(nullptr), size_(0), capacity_(0) {}

    explicit Vector(size_t count, const T& value = T())
        : data_(new T[count]), size_(count), capacity_(count) {
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = value;
        }
    }

    Vector(const Vector& other)
        : data_(other.capacity_ ? new T[other.capacity_] : nullptr),
          size_(other.size_),
          capacity_(other.capacity_) {
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }

    Vector(Vector&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~Vector() {
        delete[] data_;
    }

    Vector& operator=(const Vector& other) {
        if (this != &other) {
            T* new_data = other.capacity_ ? new T[other.capacity_] : nullptr;
            for (size_t i = 0; i < other.size_; ++i) {
                new_data[i] = other.data_[i];
            }
            delete[] data_;
            data_ = new_data;
            size_ = other.size_;
            capacity_ = other.capacity_;
        }
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    void push_back(const T& value) {
        if (size_ == capacity_) {
            reallocate(capacity_ == 0 ? 4 : capacity_ * 2);
        }
        data_[size_++] = value;
    }

    void pop_back() {
        if (size_ == 0) {
            throw std::out_of_range("Vector is empty");
        }
        --size_;
    }

    T& back() {
        if (size_ == 0) {
            throw std::out_of_range("Vector is empty");
        }
        return data_[size_ - 1];
    }

    const T& back() const {
        if (size_ == 0) {
            throw std::out_of_range("Vector is empty");
        }
        return data_[size_ - 1];
    }

    T& operator[](size_t index) {
        return data_[index];
    }

    const T& operator[](size_t index) const {
        return data_[index];
    }

    T& at(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    const T& at(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    size_t size() const noexcept { return size_; }
    size_t capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }
    void clear() noexcept { size_ = 0; }
};

// ==========================================
// 2. Стек на базе Vector (Stack)
// ==========================================
template <typename T>
class Stack {
private:
    Vector<T> container_;

public:
    Stack() = default;

    void push(const T& value) {
        container_.push_back(value);
    }

    void pop() {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }
        container_.pop_back();
    }

    T& top() {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }
        return container_.back();
    }

    const T& top() const {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }
        return container_.back();
    }

    bool empty() const noexcept {
        return container_.empty();
    }

    size_t size() const noexcept {
        return container_.size();
    }
};

// ==========================================
// 3. Очередь на двух стеках (Queue)
// ==========================================
template <typename T>
class Queue {
private:
    Stack<T> in_stack_;
    Stack<T> out_stack_;
    size_t max_capacity_; // Поддержка ограничения как у циклического буфера

    void transferIfNeeded() {
        if (out_stack_.empty()) {
            while (!in_stack_.empty()) {
                out_stack_.push(in_stack_.top());
                in_stack_.pop();
            }
        }
    }

public:
    // Если max_capacity == 0, очередь работает без жесткого лимита размера
    explicit Queue(size_t max_capacity = 0) : max_capacity_(max_capacity) {}

    void push(const T& value) {
        if (max_capacity_ > 0 && size() >= max_capacity_) {
            throw std::overflow_error("Queue buffer is full");
        }
        in_stack_.push(value);
    }

    void pop() {
        if (empty()) {
            throw std::out_of_range("Queue is empty");
        }
        transferIfNeeded();
        out_stack_.pop();
    }

    T& front() {
        if (empty()) {
            throw std::out_of_range("Queue is empty");
        }
        transferIfNeeded();
        return out_stack_.top();
    }

    const T& front() const {
        if (empty()) {
            throw std::out_of_range("Queue is empty");
        }
        const_cast<Queue*>(this)->transferIfNeeded();
        return out_stack_.top();
    }

    bool empty() const noexcept {
        return in_stack_.empty() && out_stack_.empty();
    }

    size_t size() const noexcept {
        return in_stack_.size() + out_stack_.size();
    }

    bool full() const noexcept {
        return max_capacity_ > 0 && size() >= max_capacity_;
    }
};

// ==========================================
// 4. Транслятор арифметических выражений
// ==========================================
class Translator {
public:
    enum class TokenType {
        Number,
        Operator,
        LeftParen,
        RightParen
    };

    struct Token {
        TokenType type;
        std::string text;
        double value = 0.0;
    };

private:
    std::string expression_;
    Queue<Token> infix_tokens_;
    Queue<Token> postfix_tokens_;

    static int getPriority(char op);
    static bool isOperator(char c);
    void tokenize();
    void toPostfix();

public:
    explicit Translator(const std::string& expression);

    std::string getPostfixString() const;
    double calculate();
};

#endif // TRANSLATOR_H