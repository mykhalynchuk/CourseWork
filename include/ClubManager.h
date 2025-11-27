#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Player.h"
#include "FreeAgent.h"
#include "ContractedPlayer.h"
#include "IFileHandler.h"
#include "Utils.h"

namespace FootballManagement
{
    /**
     * @file ClubManager.h
     * @brief Менеджер складу клубу, бюджету та операцій над гравцями.
     * @details Зберігає колекцію гравців, підтримує CRUD-операції, пошук/фільтрацію/сортування,
     *          підписання вільного агента, серіалізацію для збереження у файл.
     */
    class ClubManager : public IFileHandler
    {
    private:
        std::vector<std::shared_ptr<Player>> players_; ///< Склад клубу.
        double transferBudget_; ///< Трансферний/зарплатний бюджет (€).
        std::string clubName_; ///< Назва клубу.

        /**
         * @brief Згенерувати унікальний ID для нового гравця.
         */
        int GenerateUniqueId() const;

    public:
        /**
         * @brief Конструктор.
         * @param name Назва клубу.
         * @param budget Початковий бюджет (за замовчуванням 10 млн €).
         */
        explicit ClubManager(const std::string& name = "Default Club",
                             double budget = 10'000'000.0);

        /// @brief Віртуальний деструктор.
        ~ClubManager() noexcept override = default;

    public:
        [[nodiscard]] double GetTransferBudget() const;
        void SetTransferBudget(double budget);
        [[nodiscard]] std::string GetClubName() const;

    public:
        /**
         * @brief Додати гравця у склад (ID буде проставлено автоматично, якщо 0).
         */
        void AddPlayer(std::shared_ptr<Player> p);

        /**
         * @brief Вивести усіх гравців у консоль (укр.).
         */
        void ViewAllPlayers() const;

        /**
         * @brief Видалити гравця за його ID.
         */
        void RemovePlayers(int playerId);

        /**
         * @brief Відсортувати за рейтингом ефективності (спадно).
         */
        void SortByPerformanceRating();

        /**
         * @brief Пошук за підрядком у імені (регістр ігнорується).
         */
        [[nodiscard]] std::vector<std::shared_ptr<Player>>
        SearchByName(const std::string& q) const;

        /**
         * @brief Фільтрація за підрядком у статусі (регістр ігнорується).
         */
        [[nodiscard]] std::vector<std::shared_ptr<Player>>
        FilterByStatus(const std::string& status) const;

        /**
         * @brief Підписати вільного агента (списує з бюджету суму зарплати).
         * @param player Вказівник на FreeAgent.
         * @param salaryOffer Запропонована річна зарплата (€).
         * @param contractUntil Дата завершення контракту (YYYY-MM-DD) — зберігається у гравця/логах.
         * @return true, якщо підписання успішне.
         */
        bool SignFreeAgent(std::shared_ptr<FreeAgent> player,
                           double salaryOffer,
                           const std::string& contractUntil);

    public:
        /**
         * @brief Серіалізація у текст: перший рядок — "clubName,budget",
         *        далі по одному рядку на гравця (Player::Serialize()).
         */
        [[nodiscard]] std::string Serialize() const override;

        /**
         * @brief Десеріалізація шапки (назва, бюджет). Для повного завантаження складу
         *        використовуйте DeserializeAllPlayers().
         */
        void Deserialize(const std::string& data) override;

        /**
         * @brief Масове завантаження: перший рядок — клуб, решта — гравці.
         * @details Розбір конкретних підкласів гравців (FreeAgent/Goalkeeper/ContractedPlayer)
         *          залишено як TODO — залежить від формату та наявності маркерів типу.
         */
        void DeserializeAllPlayers(const std::vector<std::string>& lines);

        [[nodiscard]] const std::vector<std::shared_ptr<Player>>& GetAll() const
        {
            return players_;
        }
    };
}
