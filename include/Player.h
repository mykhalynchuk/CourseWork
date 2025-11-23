#pragma once

// === using ===
#include <string>
#include <vector>
#include "Utils.h"        // тут: enum Position/UserRole/структура Injury
#include "IFileHandler.h" // інтерфейс серіалізації

// === namespace ===
namespace FootballManagement
{
    /**
     * @file Player.h
     * @brief Оголошення абстрактного класу Player (базова сутність футболіста).
     * @details Містить антропометрію, біографію, травми та допоміжну
     *          (дефолтну) серіалізацію спільних полів. Конкретні класи
     *          повинні реалізувати чисто віртуальні методи.
     */

    /**
     * @class Player
     * @brief Абстрактний базовий клас для всіх футболістів у системі.
     * @details Підтримує копі/мув-семантику, валідацію, історію травм,
     *          а також інтерфейс IFileHandler. Повна серіалізація/десеріалізація
     *          делегується похідним класам через чисто віртуальні методи, а
     *          спільні поля серіалізуються методами SerializeBase/DeserializeBase.
     */
    class Player : public IFileHandler
    {
        // === Поля ===
    private:
        int playerId_;
        std::string name_;
        int age_;
        std::string nationality_;
        std::string origin_;
        double height_;
        double weight_;
        double marketValue_;
        bool injured_;
        std::vector<Injury> injuryHistory_;

        // === Конструктори/деструктор ===
    public:
        /// @brief Конструктор за замовчуванням.
        Player();

        /**
         * @brief Конструктор із параметрами.
         * @param name ПІБ гравця.
         * @param age Вік (додатний).
         * @param nationality Громадянство (не порожнє).
         * @param origin Походження (якщо порожнє — стане "Невідомий").
         * @param height Зріст у метрах (> 0).
         * @param weight Вага у кг (> 0).
         * @param value Ринкова вартість (≥ 0).
         */
        Player(const std::string& name, int age,
               const std::string& nationality, const std::string& origin,
               double height, double weight, double value);

        /// @brief Копіювальний конструктор.
        Player(const Player& other);

        /// @brief Переміщувальний конструктор.
        Player(Player&& other) noexcept;

        /// @brief Копіювальний оператор присвоєння.
        Player& operator=(const Player& other);

        /// @brief Переміщувальний оператор присвоєння.
        Player& operator=(Player&& other) noexcept;

        /// @brief Віртуальний деструктор (із діагностичним повідомленням).
        ~Player() noexcept override;

        // === Властивості (get/set) ===
    public:
        /// @name Гетери
        /// @{
        [[nodiscard]] int GetPlayerId() const;
        [[nodiscard]] std::string GetName() const;
        [[nodiscard]] int GetAge() const;
        [[nodiscard]] std::string GetNationality() const;
        [[nodiscard]] std::string GetOrigin() const;
        [[nodiscard]] double GetHeight() const;
        [[nodiscard]] double GetWeight() const;
        [[nodiscard]] double GetMarketValue() const;
        [[nodiscard]] bool IsInjured() const;
        [[nodiscard]] const std::vector<Injury>& GetInjuryHistory() const;
        /// @}

        /// @name Сетери
        /// @{
        void SetPlayerId(int playerId);
        void SetName(const std::string& name);
        void SetAge(int age);
        void SetNationality(const std::string& nationality);
        void SetOrigin(const std::string& origin);
        void SetHeight(double height);
        void SetWeight(double weight);
        void SetMarketValue(double value);
        /// @}

        // === Абстрактні (чисто віртуальні) методи домену ===
    public:
        /// @brief Повна інформація про гравця.
        virtual void ShowInfo() const = 0;

        /// @brief Розрахунок ринкової вартості.
        [[nodiscard]] virtual double CalculateValue() const = 0;

        /// @brief Рейтинг ефективності.
        [[nodiscard]] virtual double CalculatePerformanceRating() const = 0;

        /// @brief Статус (наприклад, "FreeAgent" / "Contracted" / "Loaned").
        [[nodiscard]] virtual std::string GetStatus() const = 0;

        /// @brief Збільшує вік на 1 рік (день народження).
        virtual void CelebrateBirthday() = 0;

        // === Загальні дії ===
    public:
        /**
         * @brief Повідомити про травму гравця.
         * @param type Тип травми (не порожній).
         * @param recoveryDays Очікувані дні відновлення (> 0).
         */
        void ReportInjury(const std::string& type, int recoveryDays);

        /// @brief Повернути гравця до форми (інжурі = false).
        void ReturnToFitness();

        /**
         * @brief Оновити ринкову вартість на відсоток (може бути від'ємним).
         * @param percentageChange Напр., 10.0 → +10%, -5.0 → -5%.
         */
        void UpdateMarketValue(double percentageChange);

        // === IFileHandler (повна серіалізація реалізується в нащадках) ===
    public:
        /// @brief Серіалізує весь об'єкт (реалізують похідні класи).
        [[nodiscard]] virtual std::string Serialize() const override = 0;

        /// @brief Десеріалізує весь об'єкт (реалізують похідні класи).
        virtual void Deserialize(const std::string& data) override = 0;

        // === Допоміжна серіалізація спільних полів для похідних класів ===
    protected:
        /**
         * @brief Серіалізує спільні поля у вигляді фрагмента JSON.
         * @return Рядок без фігурних дужок { }, лише пари ключ-значення.
         */
        [[nodiscard]] std::string SerializeBase() const;

        /**
         * @brief Десеріалізація спільних полів із JSON-фрагмента.
         * @param json JSON-фрагмент (може бути і цілий об'єкт).
         */
        void DeserializeBase(const std::string& json);
    };
} // namespace FootballManagement
