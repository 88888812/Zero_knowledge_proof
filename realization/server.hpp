
#ifndef AUTHENTICATION_PROTOCOL_SERVER_HPP
#define AUTHENTICATION_PROTOCOL_SERVER_HPP

#include "header.hpp"
#include "point.hpp"
#include "client.hpp"
#include "curve.hpp"
#include "logging.hpp"
#include "attacker.hpp"

int choose_mode() {
    int n;
    std::cout << "choose mode:\n";
    std::cout << "1.prover\n";
    std::cout << "2.attacker\n";
    std::cin >> n;
    return n;
}

Point sum(mpz_class bit_string, std::vector<Point> &Y) {
    //Yi с ai из bit string
    Point result(0, 0);
    int i = 0;
    while (bit_string) {
        Point current = Y[Y.size() - 1 - i] * (bit_string % 10);
        result += current;
        ++i;
        bit_string /= 10;
    }
    return result;
}

//шаг 10
mpz_class generate_binary_string(size_t Y_size) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(
            0, std::numeric_limits<int>::max());

    int random = dist6(rng);
    mpz_class bit_string = 0;
    while (!bit_string) {
        bit_string = random % static_cast<int>(pow(2, Y_size)); //длины m
    }
    return bit_string;
}

//шаг 13
bool check_equality(Point &R, mpz_class &s,
                    std::vector<Point> &Y, mpz_class &binary) {
    Point sG = curve.get_G() * s;
    Point d = sum(binary, Y) + sG;
    return R == d;
}


void server() {

    int n = choose_mode();
    Client *A = nullptr;
    if (n == 1) { A = new Client; }
    else if (n == 2) {
        A = new Attacker;
    } else {
        std::cerr << "Wrong input";
    }

    std::vector<Point> Y;
    //шаг 8
    Y = A->client_gives_initial_points();

    //шаг 9
    Point R = A->clients_point();

    //шаг 10
    //. Проверяющий B генерирует случайную битовую строку
    // ( a1 , ..., am)∈{0, 1}m и передает ее абоненту A.
    mpz_class bit_string = generate_binary_string(Y.size());
    //шаг10
    mpz_class binary = two_notation(bit_string);// в двоичную

    //шаг12
    mpz_class s = A->clients_summation(binary);


    //Теперь если выполнено равенство
    //шаг13
    if (check_equality(R, s, Y, binary)) {
        std::cout << "\nTHE PROOF IS ACCEPTED";
        log(true, "");
    } else {
        log(false, "");
        std::cout << "DENIED";
    }
    delete A;
}

#endif //AUTHENTICATION_PROTOCOL_SERVER_HPP
