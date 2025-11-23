#pragma once

#include <string>
#include "Player.h"
#include "Utils.h"

namespace FootballManagement
{
    /**
     * @file FieldPlayer.h
     * @brief Абстрактний клас-нащадок для всіх польових гравців (не воротарів).
     * @details Містить спільну матчеву статистику та реалізує частину поведінки.
     *          Серіалізацію/десеріалізацію та предметні формули реалізують нащадки.
     */
    class FieldPlayer : public Player
    {
        // Поля
    private:
        int totalGames_;
        int totalGoals_;
        int totalAssists_;
        int totalShots_;
        int totalTackles_;
        int keyPasses_;
        Position position_;

        // Конструктори/деструктор
    public:
        /// @brief Конструктор за замовчуванням.
        FieldPlayer();

        /**
         * @brief Конструктор з параметрами.
         */
        FieldPlayer(const std::string& name, int age,
                    const std::string& nationality, const std::string& origin,
                    double height, double weight, double marketValue,
                    Position position);

        /// @brief Копіювальний конструктор.
        FieldPlayer(const FieldPlayer& other);

        /// @brief Переміщувальний конструктор.
        FieldPlayer(FieldPlayer&& other) noexcept;

        /// @brief Копіювальний оператор присвоєння.
        FieldPlayer& operator=(const FieldPlayer& other);

        /// @brief Переміщувальний оператор присвоєння.
        FieldPlayer& operator=(FieldPlayer&& other) noexcept;

        /// @brief Віртуальний деструктор.
        ~FieldPlayer() override;

        // Властивості
    public:
        [[nodiscard]] int GetTotalGames() const;
        [[nodiscard]] int GetTotalGoals() const;
        [[nodiscard]] int GetTotalAssists() const;
        [[nodiscard]] int GetTotalShots() const;
        [[nodiscard]] int GetTotalTackles() const;
        [[nodiscard]] int GetKeyPasses() const;
        [[nodiscard]] Position GetPosition() const;

        /// @brief Встановлює ігрову позицію.
        void SetPosition(Position position);

        // Методи статистики
    public:
        /**
         * @brief Оновлює атакувальну статистику (накопичувально).
         * @throws std::invalid_argument якщо значення від’ємні.
         */
        void UpdateAttackingStats(int goals, int assists, int shots);

        /**
         * @brief Оновлює захисну статистику (накопичувально).
         * @throws std::invalid_argument якщо значення від’ємне.
         */
        void UpdateDefensiveStats(int tackles);

        /// @brief Реєструє ключовий пас.
        void RegisterKeyPass();

        /// @brief Конверсія ударів у відсотках.
        [[nodiscard]] double CalculateConversionRate() const;

        /// @brief Реєструє зіграний матч (накопичувально).
        void RegisterMatchPlayed();

        /// @brief Скидає сезонну статистику до нуля (та виводить повідомлення).
        void ResetSeasonStats();

        /// @brief Виводить інформацію про гравця (українською).
        void ShowInfo() const override;

        /// @brief День народження: збільшує вік на 1 рік (український вивід).
        void CelebrateBirthday() override;

        // Чисто віртуальні методи, які мають реалізувати нащадки
    public:
        [[nodiscard]] virtual double CalculateValue() const override = 0;
        [[nodiscard]] virtual double
        CalculatePerformanceRating() const override = 0;
        [[nodiscard]] virtual std::string GetStatus() const override = 0;
        [[nodiscard]] virtual std::string Serialize() const override = 0;
        virtual void Deserialize(const std::string& data) override = 0;
    };
} // namespace FootballManagement
