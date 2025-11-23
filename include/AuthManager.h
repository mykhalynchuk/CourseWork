#ifndef FOOTBALL_MANAGEMENT_AUTH_MANAGER_H
#define FOOTBALL_MANAGEMENT_AUTH_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "User.h"
#include "IFileHandler.h"

namespace FootballManagement
{
    /**
     * @brief Менеджер аутентифікації (Singleton).
     *
     * Зберігає зареєстрованих користувачів, керує входом/виходом,
     * підтримує гостьову сесію, а також серіалізацію/десеріалізацію
     * списку користувачів у файл.
     *
     * ВАЖЛИВО: За замовчуванням у менеджера active user = Guest,
     * але це НЕ вважається «входом», доки користувач явно не обрав
     * "Увійти як гість" (див. LoginAsGuest()).
     */
    class AuthManager final : public IFileHandler
    {
    public:
        /// Забороняємо копіювання/переміщення Singleton’а
        AuthManager(const AuthManager&) = delete;
        AuthManager& operator=(const AuthManager&) = delete;
        AuthManager(AuthManager&&) = delete;
        AuthManager& operator=(AuthManager&&) = delete;

        /**
         * @brief Отримати єдиний екземпляр менеджера (Singleton).
         */
        static AuthManager& GetInstance();

        /**
         * @brief Зареєструвати нового користувача.
         * @param userName Логін (не менше 3 символів).
         * @param password Пароль (не менше 6 символів).
         * @param userRole Роль користувача (Admin або StandardUser).
         * @return true, якщо успішно; false — якщо користувач існує або дані некоректні.
         */
        bool Register(const std::string& userName,
                      const std::string& password,
                      UserRole userRole);

        /**
         * @brief Увійти під існуючим користувачем.
         * @param userName Логін.
         * @param password Пароль.
         * @return true, якщо вхід успішний; інакше false.
         */
        bool Login(const std::string& userName,
                   const std::string& password);

        /**
         * @brief Увійти як гість (явна гостьова сесія).
         *
         * На відміну від "дефолтного" гостя в currentUser_ після запуску програми,
         * виклик цього методу означає свідомий вибір гостьового режиму.
         * У такому випадку IsLoggedIn() поверне true.
         *
         * @return Завжди true.
         */
        bool LoginAsGuest();

        /**
         * @brief Вийти з системи.
         *
         * Повертає currentUser_ до Guest і вимикає гостьову сесію.
         */
        void Logout();

        /**
         * @brief Видалити користувача (лише Admin).
         * @param userName Логін користувача.
         * @return true, якщо видалено; false — якщо не знайдено або бракує прав.
         */
        bool DeleteUser(const std::string& userName);

        /**
         * @brief Змінити роль користувача (лише Admin).
         * @param userName Логін користувача.
         * @param newRole Нова роль.
         * @return true, якщо змінено; false — якщо не знайдено або бракує прав.
         */
        bool ChangeUserRole(const std::string& userName, UserRole newRole);

        /**
         * @brief Вивести список користувачів (лише Admin).
         */
        void ViewAllUsers() const;

        /**
         * @brief Поточний користувач (Guest / User / Admin).
         */
        std::shared_ptr<User> GetCurrentUser() const;

        /**
         * @brief Повертає, чи є активний вхід у систему.
         *
         * Повертає true:
         *   1) Якщо currentUser_ не Guest (звичайний користувач/адмін), або
         *   2) Якщо Guest, але активовано гостьову сесію через LoginAsGuest().
         */
        bool IsLoggedIn() const;

        // ===== IFileHandler =====

        /**
         * @brief Серіалізувати всіх зареєстрованих користувачів у вигляді тексту.
         *
         * Формат рядків: userName:password:roleInt
         * (по одному користувачу в рядку).
         */
        [[nodiscard]] std::string Serialize() const override;

        /**
         * @brief Десеріалізація ОДНОГО користувача з одного рядка.
         *
         * Для масового завантаження юзай DeserializeAllUsers().
         */
        void Deserialize(const std::string& data) override;

        /**
         * @brief Масово десеріалізувати всіх користувачів зі списку рядків.
         * Якщо жодного не завантажено — створює дефолтного Admin (DEFAULT_ADMIN_LOGIN).
         * Після виконання поточний користувач = Guest (логіну як такого ще немає).
         */
        void DeserializeAllUsers(const std::vector<std::string>& userDatas);

    private:
        /**
         * @brief Приватний конструктор Singleton’а.
         *
         * За замовчуванням встановлює currentUser_ = Guest,
         * але guestSessionActive_ = false (НЕ вважаємо це входом).
         */
        AuthManager();

    private:
        std::unordered_map<std::string, std::shared_ptr<User>> registeredUsers_;
        std::shared_ptr<User> currentUser_;

        /**
         * @brief Прапорець «явно активований» гостьовий режим.
         *
         * false — звичайний стан після запуску (Guest за замовчуванням, НЕ «увійшов»);
         * true  — користувач свідомо обрав «Увійти як гість» (вважаємо це входом).
         */
        bool guestSessionActive_ = false;
    };

} // namespace FootballManagement

#endif // FOOTBALL_MANAGEMENT_AUTH_MANAGER_H
