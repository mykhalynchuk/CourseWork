#pragma once

// using
#include <string>

// namespace + enum/struct
#include "Utils.h"

// базові класи
#include "FieldPlayer.h"
#include "ContractDetails.h"

namespace FootballManagement
{
    /**
     * @file ContractedPlayer.h
     * @brief Клас контрактного гравця (із чинним контрактом).
     * @details Містить деталі контракту, трансферний статус та логіку
     *          продовження/розірвання контракту, трансферу і оренди.
     */
    class ContractedPlayer final : public FieldPlayer
    {
    private:
        ContractDetails contractDetails_;   ///< Деталі контракту.
        bool            listedForTransfer_; ///< Чи виставлений на трансфер.
        double          transferFee_;       ///< Мінімальна ціна трансферу (€).
        std::string     transferConditions_;///< Додаткові умови.
        std::string previousClub_;   ///< Команда/клуб, з якого перейшов.

    public:
        /// @brief Конструктор за замовчуванням.
        ContractedPlayer();

        /**
         * @brief Повний конструктор.
         * @param name Ім’я гравця.
         * @param age Вік.
         * @param nationality Національність.
         * @param origin Походження.
         * @param height Зріст (м).
         * @param weight Вага (кг).
         * @param marketValue Ринкова вартість (€).
         * @param position Позиція.
         * @param salary Зарплата (€).
         * @param contractUntil Дата завершення контракту (YYYY-MM-DD).
         */
        ContractedPlayer(const std::string& name, int age,
                         const std::string& nationality, const std::string& origin,
                         double height, double weight, double marketValue,
                         Position position, double salary,
                         const std::string& contractUntil);

        /// @brief Копіювальний конструктор.
        ContractedPlayer(const ContractedPlayer& other);

        /// @brief Переміщувальний конструктор.
        ContractedPlayer(ContractedPlayer&& other) noexcept;

        /// @brief Копіювальний оператор присвоєння.
        ContractedPlayer& operator=(const ContractedPlayer& other);

        /// @brief Переміщувальний оператор присвоєння.
        ContractedPlayer& operator=(ContractedPlayer&& other) noexcept;

        /// @brief Деструктор.
        ~ContractedPlayer() noexcept override;

        // Гетери
        [[nodiscard]] bool            IsListedForTransfer() const;
        [[nodiscard]] double          GetTransferFee() const;
        [[nodiscard]] ContractDetails GetContractDetails() const;
        [[nodiscard]] std::string GetPreviousClub() const { return previousClub_; }

        // Операції з контрактом/трансфером
        void ListForTransfer(double fee, const std::string& conditions = "");
        void RemoveFromTransferList();
        void TransferToClub(const std::string& newClub, double fee);
        void ExtendedContract(const std::string& newDate, double newSalary);
        void TerminateContract(const std::string& reason);
        void SendOnLoan(const std::string& otherClub, const std::string& endDate);
        void SetPreviousClub(const std::string& club) { previousClub_ = club; }

        // Поліморфні методи
        void         ShowInfo() const override;
        void         CelebrateBirthday() override;
        [[nodiscard]] double      CalculateValue() const override;
        [[nodiscard]] double      CalculatePerformanceRating() const override;
        [[nodiscard]] std::string GetStatus() const override;

        // Серіалізація/десеріалізація
        [[nodiscard]] std::string Serialize() const override;
        void                      Deserialize(const std::string& data) override;
    };
} // namespace FootballManagement
