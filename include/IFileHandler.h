#pragma once

#include <string>

namespace FootballManagement
{
    /**
     * @file IFileHandler.h
     * @brief Інтерфейс для класів, які підтримують серіалізацію та десеріалізацію.
     * @details Використовується як базовий контракт для збереження та відновлення
     *          даних із файлів (CSV, JSON, XML тощо).
     */
    class IFileHandler
    {
    public:
        /**
         * @brief Серіалізує об'єкт у рядкове представлення.
         * @return Рядок, придатний для збереження у файл.
         */
        [[nodiscard]] virtual std::string Serialize() const = 0;

        /**
         * @brief Десеріалізує об'єкт із рядка.
         * @param data Рядок з даними, на основі якого відновлюється об'єкт.
         */
        virtual void Deserialize(const std::string& data) = 0;

        /**
         * @brief Віртуальний деструктор.
         */
        virtual ~IFileHandler() = default;
    };
}
