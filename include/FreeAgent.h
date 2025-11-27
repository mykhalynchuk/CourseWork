#pragma once

#include <string>
#include "Utils.h"
#include "FieldPlayer.h"

namespace FootballManagement
{
    /**
     * @file FreeAgent.h
     * @brief Польовий гравець без чинного контракту (вільний агент).
     * @details Містить очікування зарплати, останній клуб, тривалість без клубу,
     *          статус доступності до переговорів і просту переговорну логіку.
     */
    class FreeAgent final : public FieldPlayer
    {
    private:
        double expectedSalary_; ///< Очікувана зарплата (€).
        std::string lastClub_; ///< Останній клуб (може бути "Невідомо").
        int monthsWithoutClub_; ///< Кількість місяців без клубу.
        bool availableForNegotiation_; ///< Чи веде переговори.

    public:
        /** @brief Конструктор за замовчуванням. */
        FreeAgent();

        /**
         * @brief Конструктор із параметрами.
         * @param name Ім’я.
         * @param age Вік.
         * @param nationality Національність.
         * @param origin Походження.
         * @param height Зріст (м).
         * @param weight Вага (кг).
         * @param marketValue Ринкова вартість (€).
         * @param position Позиція на полі.
         * @param expectedSalary Очікувана зарплата (€).
         * @param lastClub Останній клуб.
         * @throws std::invalid_argument якщо expectedSalary < 0.
         */
        FreeAgent(const std::string& name, int age,
                  const std::string& nationality, const std::string& origin,
                  double height, double weight, double marketValue,
                  Position position, double expectedSalary,
                  const std::string& lastClub);

        /** @brief Копіювальний конструктор. */
        FreeAgent(const FreeAgent& other);

        /** @brief Переміщувальний конструктор. */
        FreeAgent(FreeAgent&& other) noexcept;

        /** @brief Копіювальний оператор присвоєння. */
        FreeAgent& operator=(const FreeAgent& other);

        /** @brief Переміщувальний оператор присвоєння. */
        FreeAgent& operator=(FreeAgent&& other) noexcept;

        /** @brief Деструктор. */
        ~FreeAgent() noexcept override;

        [[nodiscard]] double GetExpectedSalary() const;
        [[nodiscard]] std::string GetLastClub() const;
        [[nodiscard]] bool IsAvailableForNegotiation() const;
        [[nodiscard]] int GetMonthWithoutClub() const;

        void SetLastClub(const std::string& lastClub);
        void SetMonthWithoutClub(int months);
        void SetAvailability(bool isAvailable);

        /**
         * @brief Переговори щодо пропозиції зарплати.
         * @param offer Запропонована клубом зарплата (€).
         * @return true, якщо гравець погодився.
         */
        bool NegotiateOffer(double offer);

        /**
         * @brief Збільшує очікувану зарплату на відсоток.
         * @param percentage Додатний відсоток.
         */
        void IncreaseExpectations(double percentage);

        /**
         * @brief Зменшує очікувану зарплату на відсоток.
         * @param percentage Додатний відсоток.
         */
        void DecreaseExpectations(double percentage);

        /**
         * @brief Підписання контракту з клубом (для прапорів вільного агента).
         * @param club Назва клубу.
         */
        void AcceptContract(const std::string& club);

        /**
         * @brief Чи є підписання вигідним.
         * @details Просте правило: value > expectedSalary * 1.3 та вік ≤ 30.
         */
        [[nodiscard]] bool IsBargain() const;

        void ShowInfo() const override;
        void CelebrateBirthday() override;
        [[nodiscard]] double CalculateValue() const override;
        [[nodiscard]] std::string GetStatus() const override;
        [[nodiscard]] double CalculatePerformanceRating() const override;


        [[nodiscard]] std::string Serialize() const override;
        void Deserialize(const std::string& data) override;
    };
}
