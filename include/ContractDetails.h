#pragma once

#include <string>

/**
 * @file ContractDetails.h
 * @brief Клас для збереження та управління інформацією про контракт футболіста.
 */

namespace FootballManagement
{
    /**
     * @class ContractDetails
     * @brief Зберігає дані про клуб, зарплату, термін дії контракту та оренду.
     */
    class ContractDetails
    {
    private:
        std::string clubName_;
        double salary_;
        std::string contractUntil_; ///< Формат: YYYY-MM-DD
        bool isLoaned_;
        std::string loanEndDate_; ///< Формат: YYYY-MM-DD або порожньо

        /// @brief Перевіряє формат дати YYYY-MM-DD.
        static bool ValidateIsoDate(const std::string& s);

    public:
        /// @brief Конструктор за замовчуванням.
        ContractDetails();

        /**
         * @brief Конструктор з параметрами.
         * @throws std::invalid_argument якщо параметри некоректні.
         */
        ContractDetails(const std::string& clubName,
                        double salary,
                        const std::string& contractUntil);

        /// @brief Копіювальний конструктор.
        ContractDetails(const ContractDetails& other);

        /// @brief Переміщувальний конструктор.
        ContractDetails(ContractDetails&& other) noexcept;

        /// @brief Копіювальний оператор присвоєння.
        ContractDetails& operator=(const ContractDetails& other);

        /// @brief Переміщувальний оператор присвоєння.
        ContractDetails& operator=(ContractDetails&& other) noexcept;

        /// @brief Деструктор за замовчуванням.
        ~ContractDetails() = default;

        [[nodiscard]] std::string GetClubName() const;
        [[nodiscard]] double GetSalary() const;
        [[nodiscard]] std::string GetContractUntil() const;
        [[nodiscard]] bool IsPlayerOnLoan() const;
        [[nodiscard]] bool IsContractValid() const;
        [[nodiscard]] bool IsExpiringSoon() const; // ≤ 6 міс від сьогодні
        [[nodiscard]] std::string GetLoanEndDate() const;

        void SetClubName(const std::string& clubName);
        void SetSalary(double salary);
        void SetContractUntil(const std::string& contractUntil);

        /**
         * @brief Переводить гравця в оренду до дати.
         * @throws std::invalid_argument якщо дата порожня або неправильна.
         */
        void SetOnLoan(const std::string& loanEndDate);

        /// @brief Повертає гравця з оренди.
        void ReturnFromLoan();

        /**
         * @brief Змінює зарплату на відсоток (може бути відʼємним).
         * @throws std::logic_error якщо поточна зарплата ≤ 0.
         */
        void AdjustSalary(double percentage);

        /**
         * @brief Продовжує контракт до нової дати.
         * @throws std::invalid_argument якщо формат дати неправильний.
         */
        void ExtendContractDate(const std::string& newDate);

        /// @brief Виводить інформацію про контракт у консоль (укр.).
        void ShowDetails() const;
    };
}
