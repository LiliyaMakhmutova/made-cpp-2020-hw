#pragma once
#include <vector>
#include <iostream>
#include <exception>

namespace task {

    struct DifferentDimensionsException : public std::exception 
    {
        const char* what() const throw () 
        {
            return "Vectors have different dimension";
        }
    };

    struct WrongDimensionsException : public std::exception
    {
        const char* what() const throw ()
        {
            return "Vectors have wrong dimension";
        }
    };

    template <typename T> bool check_if_zero(const std::vector<T>& v)
    {
        for (const auto& iter : v)
        {
            if (iter != 0) { return false; }
        }

        return true;
    }

    std::vector<double> operator+(const std::vector<double>& lhs, const std::vector<double>& rhs) 
    {
        size_t n1 = lhs.size();
        size_t n2 = rhs.size();

        if (n1 != n2) { throw DifferentDimensionsException(); }

        std::vector<double> result(n1);
        for (size_t i{ 0 }; i < n1; i++)
        {
            result[i] = lhs[i] + rhs[i];
        }

        return result;
    }

    std::vector<double> operator+(const std::vector<double>& v)
    {      
        return v;
    }

    std::vector<double> operator-(const std::vector<double>& lhs, const std::vector<double>& rhs)
    {
        size_t n1 = lhs.size();
        size_t n2 = rhs.size();

        if (n1 != n2) { throw DifferentDimensionsException(); }

        std::vector<double> result(n1);
        for (size_t i{ 0 }; i < n1; i++)
        {
            result[i] = lhs[i] - rhs[i];
        }

        return result;
    }

    std::vector<double> operator-(const std::vector<double>& v)
    {
        size_t n = v.size();
        std::vector<double> result(n);

        for (size_t i{ 0 }; i < n; i++)
        {
            result[i] = -1.0 * v[i];
        }

        return result;
    }

    double operator*(const std::vector<double>& lhs, const std::vector<double>& rhs)
    {
        size_t n1 = lhs.size();
        size_t n2 = rhs.size();

        if (n1 != n2) { throw DifferentDimensionsException(); }

        double result = 0.0;
        for (size_t i{ 0 }; i < n1; i++)
        {
            result += lhs[i] * rhs[i];
        }

        return result;
    }

    std::vector<double> operator%(const std::vector<double>& lhs, const std::vector<double>& rhs)
    {
        size_t n1 = lhs.size();
        size_t n2 = rhs.size();

        if (n1 != n2) { throw DifferentDimensionsException(); }

        if (n1 != 3) { throw WrongDimensionsException(); }

        std::vector<double> result(3);
        result[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1];
        result[1] = -(lhs[0] * rhs[2] - lhs[2] * rhs[0]);
        result[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];             

        return result;
    }

    bool are_equal(double a, double b)
    {
        double diff = a - b; 
        return (diff < 1e-12) && (diff > -1e-12);
    }

    bool operator||(const std::vector<double>& lhs, const std::vector<double>& rhs)
    {
        size_t n1 = lhs.size();
        size_t n2 = rhs.size();

        if (n1 != n2) { throw DifferentDimensionsException(); }

        // Любую пару векторов, один из которых равен нулевому вектору будем считать коллинеарными.
        if (check_if_zero(lhs) || check_if_zero(rhs)) { return true; }
                
        double coef = lhs[0] / rhs[0];
        for (size_t i{ 1 }; i < n1; i++)
        {
            if ((lhs[i] == 0 && rhs[i] != 0) || (rhs[i] == 0 && lhs[i] != 0)) { return false; }
            if (!are_equal(lhs[i] / rhs[i], coef)) { return false; }
        }

        return true;
    }
    
    bool operator&&(const std::vector<double>& lhs, const std::vector<double>& rhs)
    {
        size_t n1 = lhs.size();
        size_t n2 = rhs.size();

        return (lhs || rhs) && (lhs * rhs > 0);
    }
    
    std::istream& operator>>(std::istream& is, std::vector<double>& v)
    {
        size_t sz;
        is >> sz;

        v.resize(sz);

        for (int i = 0; i < sz; i++)
        {
            is >> v[i];
        }

        return is;
    }

    std::ostream& operator<<(std::ostream& os, const std::vector<double>& v)
    {
        for (const auto & iter : v)
        {
            os << iter << " ";
        }
        os << "\n";

        return os;
    }

    void reverse(std::vector<double>& v)
    {
        size_t n{ v.size() };
        for (int i = 0; i < n / 2; i++)
        {
            double tmp{v[i]};
            v[i] = v[n - 1 - i];
            v[n - 1 - i] = tmp;
        }
    }

    std::vector<int> operator|(const std::vector<int>& lhs, const std::vector<int>& rhs)
    {
        size_t n1 = lhs.size();
        size_t n2 = rhs.size();

        if (n1 != n2) { throw DifferentDimensionsException(); }

        std::vector<int> result(n1);
        for (size_t i{ 0 }; i < n1; i++)
        {
            result[i] = lhs[i] | rhs[i];
        }

        return result;
    }

    std::vector<int> operator&(const std::vector<int>& lhs, const std::vector<int>& rhs)
    {
        size_t n1 = lhs.size();
        size_t n2 = rhs.size();

        if (n1 != n2) { throw DifferentDimensionsException(); }

        std::vector<int> result(n1);
        for (size_t i{ 0 }; i < n1; i++)
        {
            result[i] = lhs[i] & rhs[i];
        }

        return result;
    }
}  