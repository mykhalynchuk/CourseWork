#pragma once

#include <string>

namespace FootballManagement
{
    /**
     * @file Utils.h
     * @brief Допоміжні структури, константи та перелічення для системи керування футбольним клубом.
     */

    /// @brief Ім'я файлу, у якому зберігаються користувачі.
    constexpr const char* USERS_FILE_NAME   = "users.txt";

    /// @brief Ім'я файлу, у якому зберігаються дані гравців.
    const std::string PLAYERS_FILE_NAME = "players.json";

    /// @brief Логін адміністратора за замовчуванням.
    const std::string DEFAULT_ADMIN_LOGIN = "admin";

    /// @brief Пароль адміністратора за замовчуванням.
    const std::string DEFAULT_ADMIN_PASSWORD = "admin123";

    constexpr const char* PLAYERS_DATA_FILE = "players.txt";

    /**
     * @enum UserRole
     * @brief Визначає рівень доступу користувача в системі.
     */
    enum class UserRole
    {
        Admin, ///< Адміністратор системи
        StandardUser, ///< Звичайний користувач
        Guest ///< Гостьовий режим
    };

    /**
     * @enum Position
     * @brief Перелік ігрових позицій у футболі.
     */
    enum class Position
    {
        Goalkeeper, ///< Воротар
        Defender, ///< Захисник
        Midfielder, ///< Півзахисник
        Forward ///< Нападник
    };

    /**
     * @struct Injury
     * @brief Представляє інформацію про травму футболіста.
     */
    struct Injury
    {
        std::string injuryType;
        ///< Тип травми (наприклад, "розтягнення", "перелом").
        std::string dateOccurred;
        ///< Дата, коли сталася травма (у форматі YYYY-MM-DD).
        int recoveryDays; ///< Орієнтовна кількість днів на відновлення.
    };
}
