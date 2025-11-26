#pragma once

#include <memory>
#include "ClubManager.h"
#include "FileManager.h"

/**
 * @file Menu.h
 * @brief Оголошення класу текстового меню системи керування футбольним клубом.
 */

namespace FootballManagement
{
   /**
    * @class Menu
    * @brief Реалізує текстове меню застосунку.
    *
    * Функції:
    * - автентифікація користувача;
    * - робота з гравцями (CRUD);
    * - пошук, сортування, фільтрація;
    * - підписання вільних агентів;
    * - керування користувачами (для адміністратора);
    * - збереження/завантаження даних.
    */
   class Menu
   {
   // === Поля ===
   private:
      std::shared_ptr<ClubManager> clubManager_; ///< Менеджер складу клубу.
      FileManager fileManager_;                  ///< Менеджер файлів (users/players).
      bool isRunning_;                           ///< Прапорець основного циклу.

   // === Конструктори/деструктор ===
   public:
      /**
       * @brief Конструктор.
       * @param clubManager Спільний вказівник на ClubManager.
       */
      explicit Menu(std::shared_ptr<ClubManager> clubManager);

      /**
       * @brief Віртуальний деструктор.
       */
      virtual ~Menu() = default;

   // === Властивості (public) ===
   public:
      /**
       * @brief Запустити програму: завантаження, логін, головний цикл.
       */
      void Run();

      /**
       * @brief Зупинити головний цикл.
       */
      void Stop();

      /**
       * @brief Завантажити всі дані з файлів.
       */
      void LoadAllData() const;

      /**
       * @brief Зберегти всі дані у файли.
       */
      void SaveAllData() const;

   // === Методи (public/private) ===
   private:
      /**
       * @brief Автентифікація користувача (логін/реєстрація/гість).
       */
      void authenticateUser();

      /**
       * @brief Показати головне меню.
       */
      void displayMainMenu() const;

      /**
       * @brief Обробити вибір пункту головного меню.
       * @param choice Обраний пункт.
       */
      void handleMainMenu(int choice);

      /**
       * @brief Відобразити довідку (Help).
       */
      void displayHelp() const;

      /**
       * @brief Запит підтвердження, зберегти і вийти.
       */
      void saveAndExit();

      // --- Підменю гравців/CRUD ---
      /**
       * @brief Показати підменю керування гравцями (CRUD).
       */
      void displayPlayerMenu() const;

      /**
       * @brief Додати нового гравця (покроковий ввід).
       */
      void addPlayerFlow();

      /**
       * @brief Видалити гравця за ID.
       */
      void deletePlayerFlow() const;

      /**
       * @brief Редагувати базові дані гравця (ім’я, вік, національність, вартість).
       */
      void editPlayerFlow();

      /**
       * @brief Переглянути всіх гравців.
       */
      void viewPlayersFlow() const;

      // --- Пошук/сортування/фільтрація ---
      /**
       * @brief Пошук за ім'ям, сортування за рейтингом, фільтрація за статусом.
       */
      void searchPlayerFlow() const;

      // --- Трансферний ринок ---
      /**
       * @brief Пошук вільного агента і підписання контракту.
       */
      void transferFlow() const;

      // --- Користувачі (адмін) ---
      /**
       * @brief Підменю керування користувачами (admin).
       */
      void manageUsersFlow();

      /**
       * @brief Створення користувача (admin).
       */
      void adminCreateUser();

      // --- Нове: рейтинг/стати/вартість ---
      /**
       * @brief Показати рейтинг кожному гравцю та відсортувати склад.
       */
      void RatingShowAndSortFlow();

      /**
       * @brief Змінити ринкову вартість на відсоток (одному або всім).
       */
      void AdjustMarketValuePercentFlow();

      /**
       * @brief Редагувати ігрові стати (шаблон: GK / польові).
       * @note Виклики конкретних Set-методів у похідних позначено TODO.
       */
      void EditPlayerStatsFlow();
   };
} // namespace FootballManagement
