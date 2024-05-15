#include "account.h"

#include <gmpxx.h>
#include <openssl/sha.h>

#include <QString>
#include <string>

#if ACCOUNT_DEBUG == 1
#include <cassert>
#endif

using bms::Account;

QString Account::name() const { return m_name; }

QString Account::passwd() const { return m_passwd; }

QString Account::location() const { return m_location; }

QString Account::id() const { return m_id; }

mpf_class Account::balance() const { return m_balance; }

mpf_class Account::interestRate() const { return m_interestRate; }

QString Account::cardNumber() const { return m_cardNumber; }

void Account::setName(const QString& name) { m_name = name; }

void Account::setPasswd(const QString& passwd) {
    m_passwd = hashSHA256(passwd);
}

void Account::setLocation(const QString& location) { m_location = location; }

void Account::setId(const QString& id) { m_id = id; }

void Account::setInterestRate(double rate) { m_interestRate = rate; }

void Account::transfer(Account& to, const mpf_class& amount) {
    if (m_balance >= amount) {
        m_balance -= amount;
        to.m_balance += amount;
#if ACCOUNT_DEBUG == 1
        qDebug() << "Transfer successful!";
#endif
    } else {
        std::string msg = "Transfer failed: amount(" + mpf_class2str(amount) +
                          ") is more than balance(" + mpf_class2str(m_balance) +
                          ")";
        throw std::runtime_error(msg);
    }
}

QString Account::hashSHA256(const QString& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];  // 32 字节的哈希值
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.toStdString().c_str(), str.length());
    SHA256_Final(hash, &sha256);
    char hex[2 * SHA256_DIGEST_LENGTH + 1];  // 64 字节的十六进制字符串
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hex + 2 * i, "%02x", hash[i]);  // 两个字符表示一个字节
    }
    hex[2 * SHA256_DIGEST_LENGTH] = 0;  // 字符串结尾
    return QString(hex);
}

// To: Sour_xuanzi
Account::Account(const QString& name, const QString& passwd,
                 const QString& location, const QString& id)
    : m_name(name),
      m_passwd(hashSHA256(passwd)),
      m_location(location),
      m_id(id),
      m_cardNumber(generateCardNumber()),
      m_balance(0),
      m_interestRate(0) {}

QString Account::generateCardNumber() {
    QString cardNumber;
    const int len = 16;

    // 检查处理器是否支持 rdrand 指令
    bool rdrandSupported = false;
    unsigned int eax, ebx, ecx, edx;
    __asm__ volatile("cpuid"
                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                     : "a"(1));
    if (ecx & 0x40000000) {
        rdrandSupported = true;
    }

    if (rdrandSupported) {
        // 使用 rdrand 生成随机数
        while (cardNumber.length() < len) {
            unsigned int cardNumberInt;
            unsigned char success;
            __asm__ volatile("rdrand %0; setc %1"
                             : "=r"(cardNumberInt), "=qm"(success));
            if (success) {
                cardNumber += QString::number(cardNumberInt % 10);
            }
        }
    } else {
        // 使用标准库生成随机数
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 9);
        for (int i = 0; i < len; ++i) {
            cardNumber += QString::number(dis(gen));
        }
    }

    return cardNumber;
}

void Account::display() const {
    U8ENCODING
    qDebug() << m_name << ' ' << m_id << ' ' << m_passwd << ' ' << m_cardNumber
             << ' ' << m_location;
}

void Account::deposit(const mpf_class& amount) { m_balance += amount; }

std::string Account::mpf_class2str(const mpf_class& number) {
    mp_exp_t expo;
    std::string str = number.get_str(expo);
    // 转换成标准浮点数
    str.insert(expo, ".");
    return str;
}
