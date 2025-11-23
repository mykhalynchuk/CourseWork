#include "../include/User.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace FootballManagement
{
    // === Конструктори/оператори ===

    User::User()
        : userName_("guest"),
          password_(""),
          userRole_(UserRole::Guest)
    {
    }

    User::User(const std::string& userName, const std::string& password,
               UserRole userRole)
        : userName_(userName),
          password_(password),
          userRole_(userRole)
    {
        if (userName_.empty())
            throw std::invalid_argument("Логін не може бути порожнім.");
        if (userRole_ != UserRole::Guest && password_.size() < 6)
            throw std::runtime_error("Пароль має містити щонайменше 6 символів.");
    }

    User::User(const User& other)
        : userName_(other.userName_),
          password_(other.password_),
          userRole_(other.userRole_)
    {
    }

    User::User(User&& other) noexcept
        : userName_(std::move(other.userName_)),
          password_(std::move(other.password_)),
          userRole_(other.userRole_)
    {
        other.userRole_ = UserRole::Guest;
    }

    User& User::operator=(const User& other)
    {
        if (this != &other)
        {
            userName_ = other.userName_;
            password_ = other.password_;
            userRole_ = other.userRole_;
        }
        return *this;
    }

    User& User::operator=(User&& other) noexcept
    {
        if (this != &other)
        {
            userName_ = std::move(other.userName_);
            password_ = std::move(other.password_);
            userRole_ = other.userRole_;
            other.userRole_ = UserRole::Guest;
        }
        return *this;
    }

    User::~User() noexcept
    {
        std::cout << "[ДЕБАГ] Користувача \"" << userName_ << "\" знищено.\n";
    }

    // === Властивості ===

    std::string User::GetUserName() const { return userName_; }
    std::string User::GetPassword() const { return password_; }
    UserRole User::GetUserRole() const { return userRole_; }

    void User::SetUserName(const std::string& userName)
    {
        if (userName.empty())
            throw std::invalid_argument("Логін не може бути порожнім.");
        userName_ = userName;
    }

    // === Бізнес-логіка ===

    bool User::IsAdmin() const
    {
        return userRole_ == UserRole::Admin;
    }

    bool User::ChangePassword(const std::string& oldPassword,
                              const std::string& newPassword)
    {
        if (!VerifyPassword(oldPassword))
        {
            std::cout << "[ПОМИЛКА] Невірний поточний пароль.\n";
            return false;
        }

        if (newPassword.length() < 6)
        {
            std::cout <<
                "[ПОМИЛКА] Новий пароль занадто короткий (мінімум 6 символів).\n";
            return false;
        }

        password_ = newPassword;
        std::cout << "[УСПІХ] Пароль для користувача \"" << userName_
            << "\" змінено.\n";
        return true;
    }

    bool User::VerifyPassword(const std::string& inputPassword) const
    {
        return password_ == inputPassword;
    }

    void User::SetRole(UserRole newRole)
    {
        userRole_ = newRole;
        std::cout << "[ІНФО] Роль користувача \"" << userName_
            << "\" встановлено.\n";
    }

    void User::ShowUserInfo() const
    {
        std::string roleStr;
        switch (userRole_)
        {
        case UserRole::Admin: roleStr = "Адміністратор";
            break;
        case UserRole::StandardUser: roleStr = "Користувач";
            break;
        default: roleStr = "Гість";
            break;
        }

        std::cout << "Користувач: " << userName_
            << " | Роль: " << roleStr << "\n";
    }

    void User::Logout()
    {
        std::cout << "[ІНФО] Користувач \"" << userName_
            << "\" вийшов із системи.\n";
    }

    // === Серіалізація/десеріалізація ===

    std::string User::Serialize() const
    {
        // Формат: username:password:roleInt
        std::stringstream ss;
        ss << userName_ << ":" << password_ << ":"
            << static_cast<int>(userRole_);
        return ss.str();
    }

    void User::Deserialize(const std::string& data)
    {
        try
        {
            std::stringstream ss(data);
            std::string token;
            std::vector<std::string> parts;

            while (std::getline(ss, token, ':'))
                parts.push_back(token);

            if (parts.size() != 3)
                throw std::runtime_error(
                    "Некоректний формат рядка користувача (очікується 3 частини).");

            userName_ = parts[0];
            password_ = parts[1];

            int roleInt = std::stoi(parts[2]);
            switch (roleInt)
            {
            case 0: userRole_ = UserRole::Admin;
                break;
            case 1: userRole_ = UserRole::StandardUser;
                break;
            case 2: userRole_ = UserRole::Guest;
                break;
            default: throw std::out_of_range("Невідома роль.");
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "[ПОМИЛКА] Не вдалося десеріалізувати користувача: "
                << e.what() << "\n";
            // Встановлюємо безпечні значення
            userName_ = "guest";
            password_.clear();
            userRole_ = UserRole::Guest;
        }
    }
} // namespace FootballManagement
