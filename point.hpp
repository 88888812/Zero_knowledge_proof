//
// Created by ndp20 on 21.08.2021.
//

#ifndef AUTHENTICATION_PROTOCOL_POINT_HPP
#define AUTHENTICATION_PROTOCOL_POINT_HPP


#include "header.hpp"

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

mpz_class a = 2;// коэффициент кривой
mpz_class p("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141", 16);


class point {
public:
    point() { x = 0, y = 0; };

    point(mpz_class xx, mpz_class yy) { x = xx, y = yy; };
    mpz_class x, y;


    bool operator==(point B) const {
        if (B.x == this->x && B.y == this->y) return true;
        return false;
    }

    point operator=(point B) {
        this->y = B.y;
        this->x = B.x;
        return *this;
    }

    point operator+(point &B) const {
        point A;
        mpz_class alfa;
        mpz_class inverse;
        if (B.x == 0 && B.y == 0) {
            A.y = this->y;
            A.x = this->x;
            return A;
        }
        if (this->x == 0 && this->y == 0) {
            A.y = B.y;
            B.x = 0;
            return A;
        }

        if (this->x == B.x && this->y == -B.y) {
            A.x = 0;
            A.y = 0;
            return A;
        }
        if (*this == B) {
            mpz_class double_y = 2 * B.y;
            mpz_invert(inverse.get_mpz_t(), p.get_mpz_t(), double_y.get_mpz_t());
            alfa = (3 * B.x * B.x + a) * inverse;
        } else {
            mpz_class diff = B.x - x;
            mpz_invert(inverse.get_mpz_t(), p.get_mpz_t(), diff.get_mpz_t());
            alfa = (B.y - this->y) * inverse;
        }
        if (alfa == 0) {
            A.x = 0;
            A.y = 0;
            return A;
        }

        A.x = (alfa * alfa - this->x - B.x) % p;
        A.y = (alfa * (this->x - A.x) - this->y) % p;
        if (A.x < 0) A.x += p;
        if (A.y < 0) A.y += p;
        return A;
    }

    point operator+=(point &B) {
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
            mpz_invert(inverse.get_mpz_t(), p.get_mpz_t(), double_y.get_mpz_t());
            alfa = (3 * B.x * B.x + a) * inverse;
        } else {
            mpz_class diff = B.x - x;
            mpz_invert(inverse.get_mpz_t(), p.get_mpz_t(), diff.get_mpz_t());
            alfa = (B.y - this->y) * inverse;
        }
        if (alfa == 0) {
            this->x = 0;
            this->y = 0;
            return *this;
        }

        mpz_class xx = this->x;
        this->x = (alfa * alfa - this->x - B.x) % p;
        this->y = (alfa * (xx - this->x) - this->y) % p;
        if (this->x < 0) this->x += p;
        if (this->y < 0) this->y += p;
        return *this;
    }

    point operator*(const mpz_class& k) const {
        if (k==0){return {0,0};}
        mpz_class k2 = two_notation(k);// в 2 сс

        point Y;
        std::vector<point> templ;
        templ.push_back(*this);

        int i = 1;
        mpz_class k_2 = k2;
        while (k_2 != 0 && k_2 != 1) //в 2 сс
        {
            Y = templ[i - 1] + templ[i - 1];
            templ.push_back(Y);
            // вектор сложенных частей (G,2G,4G, и тд степени двойки)-
            // max степень - размер xi
            k_2 /= 10;
            ++i;
        }
        //теперь из этих степеней двойки собираем наше число
        point result;
        bool init = false; //первая точка не плюсуется с (0,0) а просто приравнивается
        for (int j = 0; j < templ.size(); ++j) {
            if (k2 % 10 != 0) {
                if (!init) {
                    result = templ[j];
                    init = true;
                } else {
                    result += templ[j];
                }
            }
            k2 /= 10;
        }
        return result;
    }
};

//Расширенный Алгоритм Евклида
/*int inverse(int mod, int num) {
    while(num<0) num+=mod;
    if (num==0) return 0;
    if (num >= mod) {
        while (num>=mod)
            num-=mod;
    }
    int x = 1, y = 0;
    int x1 = 0, y1 = 1;
    int q;
    int module = mod;
    int num_to_inverse = num;
    int i = 0;
    while (mod != 1 && num != 1) {
        if (mod > num)q = mod / num;
        else q = num / mod;
        if (i % 2 == 0) {
            x = x - q * x1;
            y = y - q * y1;
            mod = x * module + y * num_to_inverse;
        } else {
            x1 = x1 - q * x;
            y1 = y1 - q * y;
            num = x1 * module + y1 * num_to_inverse;
        }
        ++i;
    }
    if (mod == 1) {
        if (y < 0)y += module;
        return y;
    } else {
        if (y1 < 0)y1 += module;
        return y1;
    }
}*/

#endif //AUTHENTICATION_PROTOCOL_POINT_HPP
