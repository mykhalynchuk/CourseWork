#pragma once

// using
#include <string>
#include <vector>

// базові залежності
#include "IFileHandler.h"
#include "Utils.h"

namespace FootballManagement
{
    /**
     * @file FileManager.h
     * @brief Клас для роботи з файлами: збереження, читання, перевірка існування,
     *        створення каталогу даних, побудова повного шляху та видалення файлу.
     */
    class FileManager
    {
        // Поля
    private:
        std::string directoryPath_ = "data/"; ///< Базовий каталог даних.

        // Конструктор/деструктор
    public:
        FileManager() = default;
        ~FileManager() = default;

        // Властивості
        /**
         * @brief Повертає повний шлях до файлу в каталозі даних.
         * @param fileName Ім'я файлу (може бути відносним).
         * @return Повний шлях як рядок.
         */
        [[nodiscard]] std::string
        GetFullPath(const std::string& fileName) const;

        // Методи
        /**
         * @brief Гарантує існування каталогу даних (створює за потреби).
         * @return true, якщо каталог існує або був успішно створений.
         */
        bool EnsureDirectoryExists() const;

        /**
         * @brief Перевіряє існування файлу в каталозі даних.
         * @param fileName Ім'я файлу.
         * @return true, якщо файл існує.
         */
        [[nodiscard]] bool FileExists(const std::string& fileName) const;

        /**
         * @brief Зберігає серіалізований об'єкт у файл.
         * @param fileName Ім'я файлу.
         * @param serializableObject Об'єкт, що реалізує IFileHandler.
         * @return true, якщо збереження успішне.
         */
        bool SaveToFile(const std::string& fileName,
                        const IFileHandler& serializableObject) const;

        /**
         * @brief Зчитує всі непорожні рядки з файлу.
         * @param fileName Ім'я файлу.
         * @return Вектор рядків (може бути порожнім).
         */
        [[nodiscard]] std::vector<std::string>
        LoadFromFile(const std::string& fileName) const;

        /**
         * @brief Виводить вміст файлу у консоль (для налагодження/демо).
         * @param fileName Ім'я файлу.
         */
        void DisplayFileContent(const std::string& fileName) const;

        /**
         * @brief Видаляє файл із каталогу даних.
         * @param fileName Ім'я файлу.
         * @return true, якщо файл було видалено.
         */
        bool DeleteFile(const std::string& fileName) const;
    };
} // namespace FootballManagement
