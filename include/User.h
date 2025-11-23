#pragma once

// using
#include <string>

// namespace + enum/struct
#include "Utils.h"

// інтерфейс серіалізації
#include "IFileHandler.h"

namespace FootballManagement
{
    /**
     * @file User.h
     * @brief Обліковий запис користувача системи.
     * @details Зберігає логін, пароль і роль. Підтримує серіалізацію/десеріалізацію.
     */
    class User : public IFileHandler
    {
        // Поля
    private:
        std::string userName_; ///< Логін користувача.
        std::string password_; ///< Пароль користувача (демо, без хешування).
        UserRole userRole_; ///< Роль: Admin / StandardUser / Guest.

        // Конструктори/деструктор
    public:
        /** @brief Конструктор за замовчуванням (гостьовий користувач). */
        User();

        /**
         * @brief Конструктор з параметрами.
         * @param userName Логін.
         * @param password Пароль.
         * @param userRole Роль (Admin / StandardUser / Guest).
         * @throws std::invalid_argument якщо логін порожній або пароль закороткий.
         */
        User(const std::string& userName, const std::string& password,
             UserRole userRole);

        /** @brief Копіювальний конструктор. */
        User(const User& other);

        /** @brief Переміщувальний конструктор. */
        User(User&& other) noexcept;

        /** @brief Копіювальний оператор присвоєння. */
        User& operator=(const User& other);

        /** @brief Переміщувальний оператор присвоєння. */
        User& operator=(User&& other) noexcept;

        /** @brief Деструктор (виводить діагностичне повідомлення). */
        ~User() noexcept override;

        // Властивості
    public:
        /** @brief Повертає логін користувача. */
        [[nodiscard]] std::string GetUserName() const;

        /** @brief Повертає пароль користувача (для демо; у продакшені — не робити). */
        [[nodiscard]] std::string GetPassword() const;

        /** @brief Повертає роль користувача. */
        [[nodiscard]] UserRole GetUserRole() const;

        /**
         * @brief Встановлює новий логін.
         * @throws std::invalid_argument якщо логін порожній.
         */
        void SetUserName(const std::string& userName);

        // Бізнес-логіка
    public:
        /** @brief Перевіряє, чи користувач має права адміністратора. */
        [[nodiscard]] bool IsAdmin() const;

        /**
         * @brief Змінює пароль.
         * @param oldPassword Поточний пароль.
         * @param newPassword Новий пароль (мін. 6 символів).
         * @return true, якщо пароль змінено.
         */
        bool ChangePassword(const std::string& oldPassword,
                            const std::string& newPassword);

        /**
         * @brief Перевіряє збіг пароля.
         * @param inputPassword Пароль для перевірки.
         * @return true, якщо збігається.
         */
        [[nodiscard]] bool VerifyPassword(
            const std::string& inputPassword) const;

        /**
         * @brief Встановлює роль користувача.
         */
        void SetRole(UserRole newRole);

        /**
         * @brief Виводить інформацію про користувача (укр.).
         */
        void ShowUserInfo() const;

        /**
         * @brief Вихід користувача із системи (повідомлення).
         */
        void Logout();

        // Серіалізація/десеріалізація
    public:
        /**
         * @brief Серіалізує у формат "username:password:roleInt".
         */
        [[nodiscard]] std::string Serialize() const override;

        /**
         * @brief Десеріалізує з формату "username:password:roleInt".
         * @details У разі помилки встановлюється гостьовий профіль.
         */
        void Deserialize(const std::string& data) override;
    };
} // namespace FootballManagement
