#pragma once

#include <string>

#include "Utils.h"
#include "FieldPlayer.h"
#include "ContractDetails.h"

namespace FootballManagement
{
    /**
     * @file ContractedPlayer.h
     * @brief Оголошення класу контрактного гравця.
     *
     * @details
     * Клас моделює гравця, який має чинний контракт із клубом:
     * зберігає деталі контракту, статус на трансферному ринку, мінімальну
     * трансферну плату, додаткові умови угоди та попередній клуб.
     *
     * @see FieldPlayer
     * @see ContractDetails
     */

    /**
     * @class ContractedPlayer
     * @brief Контрактний гравець (має активний контракт із клубом).
     * @details
     * Розширює можливості @ref FieldPlayer логікою управління контрактом:
     * виставлення на трансфер, зняття з трансферу, продовження/розірвання
     * контракту, відправлення в оренду та виконання базової ігрової логіки
     * (обчислення вартості та рейтингу, відображення інформації).
     */
    class ContractedPlayer final : public FieldPlayer
    {
    private:
        ContractDetails contractDetails_;
        ///< Деталі контракту (зарплата, дата завершення тощо).
        bool listedForTransfer_;
        ///< Прапорець: чи виставлений гравець на трансфер.
        double transferFee_;
        ///< Мінімальна трансферна ціна (€), якщо гравець у списку.
        std::string transferConditions_;
        ///< Додаткові умови трансферу (бонуси, відсотки, клаусули).
        std::string previousClub_;
        ///< Попередній клуб, з якого гравець перейшов.

    public:
        /**
         * @brief Конструктор за замовчуванням.
         * @post Створює гравця із пустими/безпечними значеннями та не виставленим на трансфер.
         */
        ContractedPlayer();

        /**
         * @brief Повний конструктор.
         * @param name Ім’я гравця.
         * @param age Вік (років).
         * @param nationality Національність.
         * @param origin Походження (країна/місто).
         * @param height Зріст (метри).
         * @param weight Вага (кг).
         * @param marketValue Ринкова вартість (€).
         * @param position Позиція на полі.
         * @param salary Річна зарплата за контрактом (€).
         * @param contractUntil Дата закінчення контракту у форматі YYYY-MM-DD.
         * @post Створює повністю ініціалізованого контрактного гравця з невиставленим трансфер-статусом.
         */
        ContractedPlayer(const std::string& name, int age,
                         const std::string& nationality,
                         const std::string& origin,
                         double height, double weight, double marketValue,
                         Position position, double salary,
                         const std::string& contractUntil);

        /**
         * @brief Копіювальний конструктор.
         * @param other Джерело копіювання.
         * @post Створює копію @p other.
         */
        ContractedPlayer(const ContractedPlayer& other);

        /**
         * @brief Переміщувальний конструктор.
         * @param other Об’єкт-джерело для переміщення.
         * @post Переміщує внутрішні ресурси з @p other, залишаючи його в коректному стані.
         */
        ContractedPlayer(ContractedPlayer&& other) noexcept;

        /**
         * @brief Копіювальний оператор присвоєння.
         * @param other Джерело копіювання.
         * @return *this
         */
        ContractedPlayer& operator=(const ContractedPlayer& other);

        /**
         * @brief Переміщувальний оператор присвоєння.
         * @param other Об’єкт-джерело для переміщення.
         * @return *this
         */
        ContractedPlayer& operator=(ContractedPlayer&& other) noexcept;

        /**
         * @brief Деструктор.
         * @details Віртуальний з боку бази та фінальний у цьому класі.
         */
        ~ContractedPlayer() noexcept override;

        /**
         * @brief Перевірити, чи виставлено гравця на трансфер.
         * @return true, якщо у списку трансферу; інакше false.
         */
        [[nodiscard]] bool IsListedForTransfer() const;

        /**
         * @brief Отримати мінімальну трансферну плату (€).
         * @return Поточний розмір мінімальної трансферної плати.
         * @note Має сенс тільки якщо @ref IsListedForTransfer повертає true.
         */
        [[nodiscard]] double GetTransferFee() const;

        /**
         * @brief Отримати копію деталей контракту.
         * @return Об’єкт @ref ContractDetails з поточними параметрами контракту.
         */
        [[nodiscard]] ContractDetails GetContractDetails() const;

        /**
         * @brief Отримати назву попереднього клубу.
         * @return Рядок із назвою клубу.
         */
        [[nodiscard]] std::string GetPreviousClub() const
        {
            return previousClub_;
        }

        /**
         * @brief Виставити гравця на трансфер.
         * @param fee Мінімальна ціна трансферу (€).
         * @param conditions (Необов’язково) Додаткові умови трансферу.
         * @post @ref IsListedForTransfer() == true, встановлено @ref transferFee_ і @ref transferConditions_.
         * @warning Логіка перевірки @p fee (невід’ємність тощо) має бути забезпечена на рівні виклику або всередині реалізації.
         */
        void ListForTransfer(double fee, const std::string& conditions = "");

        /**
         * @brief Зняти гравця з трансферного списку.
         * @post @ref IsListedForTransfer() == false, скидає пов’язані поля трансферу.
         */
        void RemoveFromTransferList();

        /**
         * @brief Виконати трансфер гравця до нового клубу.
         * @param newClub Назва клубу-покупця.
         * @param fee Фактична сума трансферу (€).
         * @post Оновлює @ref previousClub_, скидає трансферний статус; може впливати на @ref marketValue.
         * @note Доцільно викликати тільки якщо гравця виставлено на трансфер.
         */
        void TransferToClub(const std::string& newClub, double fee);

        /**
         * @brief Продовжити чинний контракт.
         * @param newDate Нова дата завершення контракту (YYYY-MM-DD).
         * @param newSalary Нова річна зарплата (€).
         * @post Оновлює @ref contractDetails_ із новими параметрами.
         */
        void ExtendedContract(const std::string& newDate, double newSalary);

        /**
         * @brief Достроково розірвати контракт.
         * @param reason Причина розірвання (службова нотатка/опис).
         * @post Скидає релевантні поля контракту; гравець формально втрачає статус контрактного.
         * @warning Подальша поведінка (переведення у вільного агента тощо) залежить від загальної бізнес-логіки застосунку.
         */
        void TerminateContract(const std::string& reason);

        /**
         * @brief Відправити гравця в оренду до іншого клубу.
         * @param otherClub Назва клубу-орендаря.
         * @param endDate Дата завершення оренди (YYYY-MM-DD).
         * @post Оновлює внутрішні поля контракту/статусу згідно з бізнес-логікою.
         */
        void SendOnLoan(const std::string& otherClub,
                        const std::string& endDate);

        /**
         * @brief Встановити попередній клуб.
         * @param club Назва клубу.
         * @post Оновлює @ref previousClub_.
         */
        void SetPreviousClub(const std::string& club) { previousClub_ = club; }

        /**
         * @brief Відобразити усю інформацію про гравця у консоль.
         * @details Перевизначає базову реалізацію @ref Player::ShowInfo().
         */
        void ShowInfo() const override;

        /**
         * @brief Збільшити вік гравця на 1 рік (день народження).
         * @details Може впливати на вартість/рейтинг згідно з логікою класу.
         */
        void CelebrateBirthday() override;

        /**
         * @brief Перерахувати приблизну ринкову вартість гравця.
         * @return Оцінена вартість (€) з урахуванням позиції/віку/статистики/контракту.
         */
        [[nodiscard]] double CalculateValue() const override;

        /**
         * @brief Обчислити узагальнений ігровий рейтинг продуктивності.
         * @return Число у зручному діапазоні (наприклад, 0..100 або 0..10).
         * @note Конкретна формула визначається реалізацією класу.
         */
        [[nodiscard]] double CalculatePerformanceRating() const override;

        /**
         * @brief Отримати короткий статус гравця.
         * @return Рядок-статус (наприклад, "Контрактний", "У списку трансферу", "В оренді").
         */
        [[nodiscard]] std::string GetStatus() const override;

        /**
         * @brief Серіалізувати гравця у рядок (наприклад, CSV/TSV/JSON-рядок).
         * @return Рядкове подання об’єкта для збереження у файл.
         * @see Deserialize
         */
        [[nodiscard]] std::string Serialize() const override;

        /**
         * @brief Десеріалізувати гравця з рядка даних.
         * @param data Рядкове подання (має відповідати формату @ref Serialize()).
         * @post Відновлює стан об’єкта з @p data.
         * @warning Рекомендовано виконувати перевірку формату/значень перед викликом.
         */
        void Deserialize(const std::string& data) override;
    };
}
