#pragma once

#include <string>

#include "Utils.h"
#include "Player.h"

namespace FootballManagement
{
    /**
     * @file Goalkeeper.h
     * @brief Клас, що представляє воротаря футбольної команди.
     * @details Містить специфічні для воротаря лічильники та формули.
     */
    class Goalkeeper final : public Player
    {
    private:
        int matchesPlayed_; ///< Зіграно матчів.
        int cleanSheets_; ///< «Сухих» матчів.
        int savesTotal_; ///< Загальна кількість сейвів.
        int goalsConceded_; ///< Пропущено голів.
        int penaltiesSaved_; ///< Відбиті пенальті.

    public:
        /// @brief Конструктор за замовчуванням.
        Goalkeeper();

        /**
         * @brief Конструктор з параметрами.
         */
        Goalkeeper(const std::string& name, int age,
                   const std::string& nationality, const std::string& origin,
                   double height, double weight, double marketValue);

        /// @brief Копіювальний конструктор.
        Goalkeeper(const Goalkeeper& other);

        /// @brief Переміщувальний конструктор.
        Goalkeeper(Goalkeeper&& other) noexcept;

        /// @brief Копіювальний оператор присвоєння.
        Goalkeeper& operator=(const Goalkeeper& other);

        /// @brief Переміщувальний оператор присвоєння.
        Goalkeeper& operator=(Goalkeeper&& other) noexcept;

        /// @brief Віртуальний деструктор.
        ~Goalkeeper() noexcept override;

    public:
        [[nodiscard]] int GetMatchesPlayed() const;
        [[nodiscard]] int GetCleanSheets() const;
        [[nodiscard]] int GetSavesTotal() const;
        [[nodiscard]] int GetGoalsConceded() const;
        [[nodiscard]] int GetPenaltiesSaved() const;

    public:
        /**
         * @brief Оновлює статистику після матчу.
         * @param goalsAgainst Пропущено голів.
         * @param saves Зроблено сейвів.
         * @throws std::invalid_argument якщо параметри від’ємні.
         */
        void UpdateMatchStats(int goalsAgainst, int saves);

        /// @brief Реєструє «сухий» матч (clean sheet).
        void RegisterCleanSheet();

        /// @brief Реєструє відбитий пенальті.
        void RegisterPenaltySave();

        /// @brief Обчислює відсоток сейвів.
        [[nodiscard]] double CalculateSavePercentage() const;

        /// @brief Чи є воротар «ветераном» (35+).
        [[nodiscard]] bool IsVeteran() const;

        /// @brief Скидає сезонну статистику до нуля (з повідомленням).
        void ResetSeasonStats();

    public:
        /// @brief Повний вивід інформації про воротаря (укр.).
        void ShowInfo() const override;

        /// @brief День народження: збільшує вік на 1 (укр.).
        void CelebrateBirthday() override;

        /// @brief Ринкова вартість, з урахуванням результатів.
        [[nodiscard]] double CalculateValue() const override;

        /// @brief Рейтинг ефективності воротаря.
        [[nodiscard]] double CalculatePerformanceRating() const override;

        /// @brief Текстовий статус (активний/травмований).
        [[nodiscard]] std::string GetStatus() const override;

        [[nodiscard]] std::string Serialize() const override;
        void Deserialize(const std::string& data) override;
    };
}
