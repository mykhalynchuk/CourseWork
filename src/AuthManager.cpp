#include "../include/AuthManager.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace FootballManagement
{
    // === Singleton ===

    AuthManager::AuthManager()
          : registeredUsers_(),
            currentUser_(std::make_shared<User>()), // Guest за замовчуванням
            guestSessionActive_(false)
    {
    }

    AuthManager& AuthManager::GetInstance()
    {
        static AuthManager instance;
        return instance;
    }

    // === Операції з користувачами ===

    bool AuthManager::Register(const std::string& userName,
                               const std::string& password,
                               UserRole userRole)
    {
        if (registeredUsers_.count(userName) > 0)
        {
            std::cout << "[ПОМИЛКА] Користувач \"" << userName
                << "\" уже існує.\n";
            return false;
        }

        if (userName.length() < 3 || password.length() < 6)
        {
            std::cout <<
                "[ПОМИЛКА] Логін має бути ≥ 3 символів, пароль ≥ 6 символів.\n";
            return false;
        }

        try
        {
            auto newUser = std::make_shared<User>(userName, password, userRole);
            registeredUsers_[userName] = newUser;
            std::cout << "[УСПІХ] Користувача \"" << userName <<
                "\" зареєстровано.\n";
            return true;
        }
        catch (const std::exception& e)
        {
            std::cout << "[ПОМИЛКА] Неможливо зареєструвати користувача: "
                << e.what() << "\n";
            return false;
        }
    }

    bool AuthManager::Login(const std::string& userName,
                            const std::string& password)
    {
        const auto it = registeredUsers_.find(userName);
        if (it == registeredUsers_.end())
        {
            std::cout << "[ПОМИЛКА] Користувача \"" << userName <<
                "\" не знайдено.\n";
            return false;
        }

        const auto& user = it->second;
        if (user->VerifyPassword(password))
        {
            currentUser_ = user;
            std::cout << "[ВХІД] Вітаємо, " << userName << "! Роль: "
                << (user->IsAdmin() ? "Адміністратор" : "Користувач")
                << ".\n";
            return true;
        }

        std::cout << "[ПОМИЛКА] Невірний пароль для користувача \"" << userName
            << "\".\n";
        return false;
    }

    bool AuthManager::LoginAsGuest()
    {
        currentUser_ = std::make_shared<User>("guest", "", UserRole::Guest);
        guestSessionActive_ = true;
        std::cout << "[ІНФО] Вхід як гість. Доступ обмежений.\n";
        return true;
    }

    void AuthManager::Logout()
    {
        if (IsLoggedIn())
        {
            std::cout << "[ІНФО] Користувач \"" << currentUser_->GetUserName()
                << "\" вийшов із системи.\n";
        }
        currentUser_ = std::make_shared<User>(); // Guest за замовчуванням
        guestSessionActive_ = false;
    }

    bool AuthManager::DeleteUser(const std::string& userName)
    {
        if (!IsLoggedIn() || !currentUser_->IsAdmin())
        {
            std::cout <<
                "[ВІДМОВА] Лише адміністратор може видаляти користувачів.\n";
            return false;
        }

        if (userName == currentUser_->GetUserName())
        {
            std::cout <<
                "[ПОМИЛКА] Неможливо видалити свій активний обліковий запис.\n";
            return false;
        }

        const std::size_t erased = registeredUsers_.erase(userName);
        if (erased > 0)
        {
            std::cout << "[УСПІХ] Користувача \"" << userName <<
                "\" видалено.\n";
            return true;
        }

        std::cout << "[ПОМИЛКА] Користувача \"" << userName <<
            "\" не знайдено.\n";
        return false;
    }

    bool AuthManager::ChangeUserRole(const std::string& userName,
                                     UserRole newRole)
    {
        if (!IsLoggedIn() || !currentUser_->IsAdmin())
        {
            std::cout <<
                "[ВІДМОВА] Тільки адміністратор може змінювати ролі.\n";
            return false;
        }

        auto it = registeredUsers_.find(userName);
        if (it == registeredUsers_.end())
        {
            std::cout << "[ПОМИЛКА] Користувача \"" << userName <<
                "\" не знайдено.\n";
            return false;
        }

        it->second->SetRole(newRole);
        std::cout << "[ІНФО] Роль користувача \"" << userName <<
            "\" змінено.\n";
        return true;
    }

    void AuthManager::ViewAllUsers() const
    {
        if (!IsLoggedIn() || !currentUser_->IsAdmin())
        {
            std::cout <<
                "[ВІДМОВА] Лише адміністратор може переглядати список користувачів.\n";
            return;
        }

        std::cout << "\n=== СПИСОК КОРИСТУВАЧІВ (" << registeredUsers_.size() <<
            ") ===\n";
        for (const auto& [name, user] : registeredUsers_)
        {
            user->ShowUserInfo();
        }
        std::cout << "===========================================\n";
    }

    std::shared_ptr<User> AuthManager::GetCurrentUser() const
    {
        return currentUser_;
    }

    bool AuthManager::IsLoggedIn() const
    {
        if (!currentUser_) return false;

        if (currentUser_->GetUserRole() != UserRole::Guest)
            return true;

        return guestSessionActive_;
    }

    std::string AuthManager::Serialize() const
    {
        std::stringstream ss;
        bool first = true;
        for (const auto& [_, user] : registeredUsers_)
        {
            if (!first) ss << "\n";
            ss << user->Serialize();
            first = false;
        }
        return ss.str();
    }

    void AuthManager::Deserialize(const std::string& data)
    {
        // Підтримуємо десеріалізацію ОДНОГО користувача з рядка.
        // Для масового завантаження — використовуйте DeserializeAllUsers().
        if (data.empty()) return;

        try
        {
            auto u = std::make_shared<User>();
            u->Deserialize(data);
            if (!u->GetUserName().empty())
            {
                registeredUsers_[u->GetUserName()] = u;
                std::cout << "[ІНФО] Додано користувача \"" << u->GetUserName()
                    << "\" з десеріалізації одного запису.\n";
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "[ПОМИЛКА] Не вдалося десеріалізувати користувача: "
                << e.what() << "\n";
        }
    }

    void AuthManager::DeserializeAllUsers(
        const std::vector<std::string>& userDatas)
    {
        registeredUsers_.clear();
        int count = 0;

        for (const auto& line : userDatas)
        {
            if (line.empty()) continue;

            try
            {
                auto u = std::make_shared<User>();
                u->Deserialize(line);

                if (!u->GetUserName().empty())
                {
                    registeredUsers_[u->GetUserName()] = u;
                    ++count;
                }
            }
            catch (const std::exception& e)
            {
                std::cout <<
                    "[ПОМИЛКА] Пропущено рядок користувача через помилку: "
                    << e.what() << "\n";
            }
        }

        if (count == 0)
        {
            std::cout << "[ПОПЕРЕДЖЕННЯ] Користувачів не знайдено. "
                "Створюється обліковий запис адміністратора за замовчуванням.\n";
            Register(DEFAULT_ADMIN_LOGIN, DEFAULT_ADMIN_PASSWORD,
                     UserRole::Admin);
        }

        std::cout << "[ІНФО] Завантажено користувачів: " << count << "\n";
        // Після масового завантаження — не залогінений (Guest)
        currentUser_ = std::make_shared<User>();
    }
} // namespace FootballManagement
