//
// Created by ndp20 on 21.08.2021.
//

#ifndef AUTHENTICATION_PROTOCOL_POINT_HPP
#define AUTHENTICATION_PROTOCOL_POINT_HPP


#include <utility>

#include "header.hpp"

/*! \brief переводит число в двоичную систему счисления
 *
 * @param num исходное число
 * @return число в 2сс
 */
mpz_class two_notation(mpz_class num) {
    //перевод из 10сс в 2сс
    // необходимо для минимизации числа итераций суммирования
    mpz_class result = 0, k = 1;

    while (num) {
        result += (num % 2) * k;
        k *= 10;
        num /= 2;
    }

    return result;
}


//! \brief класс Point выполняет действия над точками кривой
class Point {
public:
    Point() { x = 0, y = 0; };

    Point(mpz_class xx, mpz_class yy) { x = std::move(xx), y = std::move(yy); };
    mpz_class x, y;


    //! \brief сравнивает точки
    bool operator==(const Point &B) const {
        return B.x == this->x && B.y == this->y;
    }

    //! \brief приравнивает точку
    Point operator=(const Point &B);

    //! \brief суммирование
    Point operator+(Point &B) const;

    //! \brief суммирование
    Point operator+=(Point &B);

    //! \brief умножение точки на число
    Point operator*(const mpz_class &k) const;
};

Point Point::operator=(const Point &B) {
    this->y = B.y;
    this->x = B.x;
    return *this;
}

Point Point::operator+(Point &B) const {
    Point A;
    mpz_class alfa;
    mpz_class inverse = 0;
    if (B.x == 0 && B.y == 0) {
        A.y = this->y;
        A.x = this->x;
        return A;
    }
    if (this->x == 0 && this->y == 0) {
        A.y = B.y;
        A.x = B.x;
        return A;
    }

    if (this->x == B.x && this->y == -B.y) {
        A.x = 0;
        A.y = 0;
        return A;
    }
    if (*this == B) {
        mpz_class double_y = 2 * B.y;
        mpz_invert(inverse.get_mpz_t(), double_y.get_mpz_t(), p.get_mpz_t());
        alfa = ((3 * B.x * B.x + a) * inverse) % p;
    } else {
        mpz_class diff = this->x - B.x;
        mpz_invert(inverse.get_mpz_t(), diff.get_mpz_t(), p.get_mpz_t());
        alfa = ((this->y - B.y) * inverse) % p;
    }
    if (alfa == 0) {
        A.x = 0;
        A.y = 0;
        return A;
    }

    A.x = (alfa * alfa - this->x - B.x) % p;
    A.y = (alfa * (A.x - this->x) + this->y) % p;
    A.y *= -1;
    while (A.x < 0) A.x += p;
    while (A.y < 0) A.y += p;
    return A;
}

Point Point::operator+=(Point &B) {
    mpz_class alfa;
    mpz_class inverse;
    if (B.x == 0 && B.y == 0) return *this;
    if (this->x == 0 && this->y == 0) {
        this->y = B.y;
        this->x = B.x;
        return *this;
    }

    if (this->x == B.x && this->y == -B.y) {
        this->x = 0;
        this->y = 0;
        return *this;
    }
    if (*this == B) {
        mpz_class double_y = 2 * B.y;
        mpz_invert(inverse.get_mpz_t(), double_y.get_mpz_t(), p.get_mpz_t());
        alfa = ((3 * B.x * B.x + a) * inverse) % p;
    } else {
        mpz_class diff = this->x - B.x;
        mpz_invert(inverse.get_mpz_t(), diff.get_mpz_t(), p.get_mpz_t());
        alfa = ((this->y - B.y) * inverse) % p;
    }
    if (alfa == 0) {
        this->x = 0;
        this->y = 0;
        return *this;
    }

    mpz_class xx = this->x;
    mpz_class yy = this->y;
    this->x = (alfa * alfa - this->x - B.x) % p;
    this->y = (alfa * (this->x - xx) + yy) % p;
    //this->y = (alfa * (this->x - xx) + this->y) % p;
    while (this->x < 0) this->x += p;
    this->y *= -1;
    while (this->y < 0) this->y += p;
    return *this;
}

Point Point::operator*(const mpz_class &k) const {
    if (k == 0) { return {0, 0}; }
    mpz_class k2 = two_notation(k);// в 2 сс

    Point Y;
    std::vector<Point> templ;
    templ.push_back(*this);

    int i = 1;
    mpz_class k_2 = k2;
    k_2 /= 10;
    while (k_2) //в 2 сс
    {
        Y = templ[i - 1] + templ[i - 1];
        templ.push_back(Y);
        // вектор сложенных частей (G,2G,4G, и тд степени двойки)-
        // max степень - размер xi
        k_2 /= 10;
        ++i;
    }
    //теперь из этих степеней двойки собираем наше число
    Point result;
    bool init = false; //первая точка не плюсуется с (0,0) а просто приравнивается
    for (auto &j: templ) {
        if (k2 % 10 != 0) {
            if (!init) {
                result = j;
                init = true;
            } else {
                result += j;
            }
        }
        k2 /= 10;
    }
    return result;
}

#endif //AUTHENTICATION_PROTOCOL_POINT_HPP
