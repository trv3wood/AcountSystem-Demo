#include <cassert>
#include <iostream>
#include <string>
#include <gmp.h>
#include <gtest/gtest.h>
#include "encryptable.h"
#include <gmpxx.h>
class TestEncryptable : public bms::Encryptable {
public:
    void encrypt() {
        mp_exp_t exp;
        msg = num.get_str(exp);
        unsigned char ciphertext[1024];
        int ciphertext_len = encryptImpl((unsigned char *)msg.c_str(), msg.size(), key, iv, ciphertext);
        msg = std::string((char *)ciphertext, ciphertext_len);
    }
    void decrypt() {
        unsigned char plaintext[1024];
        int plaintext_len = decryptImpl((unsigned char *)msg.c_str(), msg.size(), key, iv, plaintext);
        msg = std::string((char *)plaintext, plaintext_len);
        num.set_str(msg, 10);
    }
    std::string msg;
    mpf_class num;
};

// void testMsg(const std::string &msg) {
//     TestEncryptable test;
//     test.msg = msg;
//     test.encrypt();
//     std::string encrypted = test.msg;
//     test.decrypt();
//     std::string decrypted = test.msg;
//     std::cout << "Original: " << msg << ' ' << msg.size() << "\n"
//               << "Encrypted: " << encrypted << ' ' << encrypted.size() << "\n"
//               << "Decrypted: " << decrypted << ' ' << decrypted.size() << "\n";
//     ASSERT_NE(encrypted, decrypted);
//     ASSERT_EQ(msg, decrypted);
// }

void testNum(const mpf_class &num) {
    TestEncryptable test;
    test.num = num;
    test.encrypt();
    std::string encrypted = test.msg;
    test.decrypt();
    mpf_class decrypted = test.num;
    std::cout << "Original: " << num << "\n"
              << "Encrypted: " << encrypted << "\n"
              << "Decrypted: " << decrypted << "\n";
    mp_exp_t exp;
    ASSERT_NE(encrypted, decrypted.get_str(exp));
    ASSERT_EQ(num.get_str(exp), decrypted.get_str(exp));
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

// TEST(encryptable, text) {
//     testMsg("Hello, world!");
//     testMsg("1145141919810");
//     testMsg("fhoeihfiehigg");
//     testMsg("我是山里灵活的狗");
// }

TEST(encryptable, num) {
    mpf_class num("3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679");

    testNum(num);
    testNum(num * 2);
    testNum(num / 2);
    testNum(num + 1);
}