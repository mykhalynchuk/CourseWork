#pragma once

// using
#include <memory>

// namespace / deps
#include "ClubManager.h"
#include "FileManager.h"

namespace FootballManagement
{
    /**
     * @file Menu.h
     * @brief Текстове меню керування системою: автентифікація,
     *        операції з гравцями та керування користувачами.
     */
    class Menu
    {
    // Поля
    private:
        std::shared_ptr<ClubManager> clubManager_; ///< Менеджер складу клубу.
        FileManager fileManager_;                  ///< Файловий менеджер.
        bool isRunning_;                           ///< Прапорець циклу роботи.

    // Конструктори/деструктор
    public:
        /**
         * @brief Створює меню.
         * @param clubManager Вказівник на менеджер клубу.
         */
        explicit Menu(std::shared_ptr<ClubManager> clubManager);

        /// @brief Віртуальний деструктор.
        virtual ~Menu() = default;

    // Методи (публічні)
    public:
        /// @brief Запускає цикл програми (головне меню).
        void Run();

        /// @brief Акуратно зупиняє цикл програми.
        void Stop();

        /// @brief Завантажує всі дані (користувачі та гравці).
        void LoadAllData() const;

        /// @brief Зберігає всі дані (користувачі та гравці).
        void SaveAllData() const;

    // Методи (приватні)
    private:
        /// @brief Авторизація користувача з 3 спробами.
        void authenticateUser();

        /// @brief Відображає головне меню.
        void displayMainMenu() const;

        /// @brief Обробляє вибір у головному меню.
        void handleMainMenu(int choice);

        /// @brief Відображає довідку.
        void displayHelp() const;

        /// @brief Запитує збереження та завершує роботу.
        void saveAndExit();

        /// @brief Підменю керування гравцями.
        void displayPlayerMenu() const;

        /// @brief Потік видалення гравця за ID.
        void deletePlayerFlow() const;

        /// @brief Потік додавання гравця.
        void addPlayerFlow();

        /// @brief Перегляд усіх гравців.
        void viewPlayersFlow() const;

        /// @brief Пошук/сортування/фільтрація гравців.
        void searchPlayerFlow() const;

        /// @brief Робота з «ринком» — підписання вільного агента.
        void transferFlow() const;

        /// @brief Керування користувачами (тільки Admin).
        void manageUsersFlow();

        /// @brief Створення користувача (тільки Admin).
        static void adminCreateUser();
    };
} // namespace FootballManagement
