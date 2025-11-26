#include "../include/ClubManager.h"
#include "../include/Goalkeeper.h"


#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <iomanip>
#include <cctype>

namespace FootballManagement
{
    // === Конструктор/властивості ===

    ClubManager::ClubManager(const std::string& name, double budget)
        : players_(),
          transferBudget_(budget),
          clubName_(name)
    {
        std::cout << "[ІНФО] Менеджер клубу \"" << clubName_
            << "\" ініціалізований (Бюджет: " << std::fixed <<
            std::setprecision(2)
            << transferBudget_ << " €).\n";
    }

    double ClubManager::GetTransferBudget() const { return transferBudget_; }

    void ClubManager::SetTransferBudget(double budget)
    {
        if (budget < 0.0)
            throw std::invalid_argument("Бюджет не може бути від’ємним.");
        transferBudget_ = budget;
    }

    std::string ClubManager::GetClubName() const { return clubName_; }

    // === Службові ===

    int ClubManager::GenerateUniqueId() const
    {
        int maxId = 1000;
        for (const auto& p : players_)
        {
            if (p && p->GetPlayerId() > maxId)
                maxId = p->GetPlayerId();
        }
        return maxId + 1;
    }

    // === Операції з гравцями ===

    void ClubManager::AddPlayer(std::shared_ptr<Player> p)
    {
        if (!p)
            throw std::invalid_argument("Неможливо додати порожнього гравця.");

        if (p->GetPlayerId() == 0)
            p->SetPlayerId(GenerateUniqueId());

        players_.push_back(p);
        std::cout << "[УСПІХ] Додано гравця: " << p->GetName()
            << " (ID: " << p->GetPlayerId() << ").\n";
    }

    void ClubManager::ViewAllPlayers() const
    {
        std::cout << "\n=== СКЛАД КЛУБУ \"" << clubName_ << "\" ===\n";
        if (players_.empty())
        {
            std::cout << "Немає зареєстрованих гравців.\n";
            return;
        }

        for (const auto& p : players_)
        {
            if (!p) continue;

            if (p) p->ShowInfo();
            std::cout << "--------------------------------------------\n";
        }
    }

    void ClubManager::RemovePlayers(int playerId)
    {
        const auto before = players_.size();
        players_.erase(std::remove_if(players_.begin(), players_.end(),
                                      [playerId](
                                      const std::shared_ptr<Player>& p)
                                      {
                                          return p && p->GetPlayerId() ==
                                              playerId;
                                      }),
                       players_.end());

        if (players_.size() < before)
            std::cout << "[УСПІХ] Гравця з ID " << playerId << " видалено.\n";
        else
            std::cout << "[НЕ ЗНАЙДЕНО] Гравця з ID " << playerId <<
                " немає у складі.\n";
    }

    void ClubManager::SortByPerformanceRating()
    {
        std::sort(players_.begin(), players_.end(),
                  [](const std::shared_ptr<Player>& a,
                     const std::shared_ptr<Player>& b)
                  {
                      if (!a) return false;
                      if (!b) return true;
                      return a->CalculatePerformanceRating() > b->
                          CalculatePerformanceRating();
                  });
        std::cout <<
            "[ІНФО] Гравців відсортовано за рейтингом ефективності (спадно).\n";
    }

    std::vector<std::shared_ptr<Player>>
    ClubManager::SearchByName(const std::string& q) const
    {
        std::vector<std::shared_ptr<Player>> results;

        std::string lowerQuery = q;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(),
                       [](unsigned char c)
                       {
                           return static_cast<char>(std::tolower(c));
                       });

        for (const auto& p : players_)
        {
            if (!p) continue;
            std::string name = p->GetName();
            std::transform(name.begin(), name.end(), name.begin(),
                           [](unsigned char c)
                           {
                               return static_cast<char>(std::tolower(c));
                           });

            if (name.find(lowerQuery) != std::string::npos)
                results.push_back(p);
        }
        return results;
    }

    std::vector<std::shared_ptr<Player>>
    ClubManager::FilterByStatus(const std::string& status) const
    {
        std::vector<std::shared_ptr<Player>> filtered;

        std::string lowerStatus = status;
        std::transform(lowerStatus.begin(), lowerStatus.end(),
                       lowerStatus.begin(),
                       [](unsigned char c)
                       {
                           return static_cast<char>(std::tolower(c));
                       });

        for (const auto& p : players_)
        {
            if (!p) continue;
            std::string playerStatus = p->GetStatus();
            std::transform(playerStatus.begin(), playerStatus.end(),
                           playerStatus.begin(),
                           [](unsigned char c)
                           {
                               return static_cast<char>(std::tolower(c));
                           });

            if (playerStatus.find(lowerStatus) != std::string::npos)
                filtered.push_back(p);
        }

        return filtered;
    }

    bool ClubManager::SignFreeAgent(std::shared_ptr<FreeAgent> player,
                                    double salaryOffer,
                                    const std::string& contractUntil)
    {
        if (!player)
        {
            std::cout <<
                "[ПОМИЛКА] Неможливо підписати: гравець не визначений.\n";
            return false;
        }

        if (salaryOffer <= 0.0)
        {
            std::cout << "[ПОМИЛКА] Некоректна сума зарплати.\n";
            return false;
        }

        if (salaryOffer > transferBudget_)
        {
            std::cout <<
                "[ПОМИЛКА] Недостатньо коштів у бюджеті для підписання.\n";
            return false;
        }

        if (!player->NegotiateOffer(salaryOffer))
        {
            std::cout << "[ВІДМОВА] Гравець відхилив пропозицію.\n";
            return false;
        }

        // Підписали — фіксуємо у вільного агента та оновлюємо бюджет
        player->AcceptContract(clubName_);
        transferBudget_ -= salaryOffer;

        // Якщо гравця ще не було у складі — додаємо
        bool present = false;
        for (const auto& p : players_)
        {
            if (p && p->GetPlayerId() == player->GetPlayerId())
            {
                present = true;
                break;
            }
        }
        if (!present) AddPlayer(player);

        std::cout << "[УСПІХ] Вільного агента " << player->GetName()
            << " підписано до клубу \"" << clubName_ << "\" "
            << "до " << contractUntil << ". Залишок бюджету: "
            << std::fixed << std::setprecision(2) << transferBudget_ << " €.\n";
        return true;
    }

    // === Серіалізація/десеріалізація ===

    std::string ClubManager::Serialize() const
    {
        // Рядок шапки клубу + гравці построково
        std::ostringstream ss;
        ss << clubName_ << "," << std::fixed << std::setprecision(2) <<
            transferBudget_ << "\n";
        for (const auto& p : players_)
        {
            if (!p) continue;
            ss << p->Serialize() << "\n";
        }
        return ss.str();
    }

    void ClubManager::Deserialize(const std::string& data)
    {
        // Простий розбір шапки "clubName,budget"
        if (data.empty()) return;
        try
        {
            std::stringstream ss(data);
            std::string firstLine;
            std::getline(ss, firstLine);
            if (firstLine.empty()) return;

            std::stringstream header(firstLine);
            std::string budgetStr;
            std::getline(header, clubName_, ',');
            if (std::getline(header, budgetStr))
                transferBudget_ = std::stod(budgetStr);

            std::cout << "[ІНФО] Завантажено клуб: " << clubName_
                << " | Бюджет: " << std::fixed << std::setprecision(2)
                << transferBudget_ << " €.\n";

            if (ss.peek() != std::stringstream::traits_type::eof())
            {
                std::cout <<
                    "[ПОПЕРЕДЖЕННЯ] Для повного складу використайте DeserializeAllPlayers().\n";
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "[ПОМИЛКА] Помилка під час розбору даних клубу: "
                << e.what() << "\n";
        }
    }

    void ClubManager::DeserializeAllPlayers(
        const std::vector<std::string>& lines)
    {
        players_.clear();

        if (lines.empty())
        {
            std::cout << "[ПОПЕРЕДЖЕННЯ] Порожні дані — гравців не знайдено.\n";
            return;
        }

        // 1) Шапка клубу
        try
        {
            std::stringstream ss(lines[0]);
            std::string budgetStr;
            std::getline(ss, clubName_, ',');
            if (std::getline(ss, budgetStr))
                transferBudget_ = std::stod(budgetStr);

            std::cout << "[ІНФО] Завантажено дані клубу: " << clubName_
                << " | Бюджет: " << std::fixed << std::setprecision(2)
                << transferBudget_ << " €.\n";
        }
        catch (const std::exception& e)
        {
            std::cout << "[ПОМИЛКА] Помилка при розборі шапки клубу: " << e.
                what() << "\n";
        }

        for (size_t i = 1; i < lines.size(); ++i)
        {
            const std::string& row = lines[i];
            if (row.empty()) continue;

            auto findRole = [&](const std::string& s)->std::string{
                const std::string k = "\"role\":\"";
                const auto pos = s.find(k);
                if (pos == std::string::npos) return {};
                auto start = pos + k.size();
                auto end = s.find('\"', start);
                if (end == std::string::npos) return {};
                return s.substr(start, end - start);
            };

            const std::string role = findRole(row);
            std::shared_ptr<Player> p;

            if (role == "FreeAgent") {
                auto obj = std::make_shared<FreeAgent>();
                obj->Deserialize(row);
                p = obj;
            } else if (role == "Goalkeeper") {
                auto obj = std::make_shared<Goalkeeper>();
                obj->Deserialize(row);
                p = obj;
            } else if (role == "ContractedPlayer") {
                auto obj = std::make_shared<ContractedPlayer>();
                obj->Deserialize(row);
                p = obj;
            } else {
                std::cout << "[ПОПЕРЕДЖЕННЯ] Невідомий role, рядок пропущено.\n";
                continue;
            }

            // Підіймаємо id, якщо 0
            if (p->GetPlayerId() == 0) p->SetPlayerId(GenerateUniqueId());
            players_.push_back(std::move(p));

        }
    } // namespace FootballManagement
}