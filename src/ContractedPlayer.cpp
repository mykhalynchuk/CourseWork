#include "../include/ContractedPlayer.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace FootballManagement
{
    // === Конструктори/оператори ===

    ContractedPlayer::ContractedPlayer()
        : FieldPlayer(),
          contractDetails_(),
          listedForTransfer_(false),
          transferFee_(0.0),
          transferConditions_("")
    {
    }

    ContractedPlayer::ContractedPlayer(const std::string& name, int age,
                                       const std::string& nationality,
                                       const std::string& origin,
                                       double height, double weight,
                                       double marketValue,
                                       Position position,
                                       double salary,
                                       const std::string& contractUntil)
        : FieldPlayer(name, age, nationality, origin, height, weight,
                      marketValue, position),
          contractDetails_("Невідомо", salary, contractUntil),
          // клуб уточнюється пізніше
          listedForTransfer_(false),
          transferFee_(0.0),
          transferConditions_("")
    {
    }

    ContractedPlayer::ContractedPlayer(const ContractedPlayer& other)
        : FieldPlayer(other),
          contractDetails_(other.contractDetails_),
          listedForTransfer_(other.listedForTransfer_),
          transferFee_(other.transferFee_),
          transferConditions_(other.transferConditions_)
    {
    }

    ContractedPlayer::ContractedPlayer(ContractedPlayer&& other) noexcept
        : FieldPlayer(std::move(other)),
          contractDetails_(std::move(other.contractDetails_)),
          listedForTransfer_(other.listedForTransfer_),
          transferFee_(other.transferFee_),
          transferConditions_(std::move(other.transferConditions_))
    {
        other.listedForTransfer_ = false;
        other.transferFee_ = 0.0;
        other.transferConditions_.clear();
    }

    ContractedPlayer& ContractedPlayer::operator=(const ContractedPlayer& other)
    {
        if (this != &other)
        {
            FieldPlayer::operator=(other);
            contractDetails_ = other.contractDetails_;
            listedForTransfer_ = other.listedForTransfer_;
            transferFee_ = other.transferFee_;
            transferConditions_ = other.transferConditions_;
        }
        return *this;
    }

    ContractedPlayer& ContractedPlayer::operator=(
        ContractedPlayer&& other) noexcept
    {
        if (this != &other)
        {
            FieldPlayer::operator=(std::move(other));
            contractDetails_ = std::move(other.contractDetails_);
            listedForTransfer_ = other.listedForTransfer_;
            transferFee_ = other.transferFee_;
            transferConditions_ = std::move(other.transferConditions_);

            other.listedForTransfer_ = false;
            other.transferFee_ = 0.0;
            other.transferConditions_.clear();
        }
        return *this;
    }

    ContractedPlayer::~ContractedPlayer() noexcept
    {
        std::cout << "[ДЕБАГ] Контрактного гравця \"" << GetName() <<
            "\" видалено.\n";
    }

    // === Гетери ===
    bool ContractedPlayer::IsListedForTransfer() const
    {
        return listedForTransfer_;
    }

    double ContractedPlayer::GetTransferFee() const { return transferFee_; }

    ContractDetails ContractedPlayer::GetContractDetails() const
    {
        return contractDetails_;
        // копія; за потреби можна додати геттер-посилання
    }

    // === Операції з контрактом/трансфером ===
    void ContractedPlayer::ListForTransfer(double fee,
                                           const std::string& conditions)
    {
        if (fee <= 0.0)
            throw std::invalid_argument("Трансферна сума має бути додатною.");

        listedForTransfer_ = true;
        transferFee_ = fee;
        transferConditions_ = conditions;

        std::cout << "[ІНФО] " << GetName()
            << " виставлений на трансфер. Мінімальна сума: "
            << std::fixed << std::setprecision(2) << fee << " €.\n";
    }

    void ContractedPlayer::RemoveFromTransferList()
    {
        listedForTransfer_ = false;
        transferFee_ = 0.0;
        transferConditions_.clear();

        std::cout << "[ІНФО] " << GetName() <<
            " знятий із трансферного списку.\n";
    }

    void ContractedPlayer::TransferToClub(const std::string& newClub,
                                          double fee)
    {
        if (!listedForTransfer_)
        {
            std::cout << "[ПОМИЛКА] Гравець не виставлений на трансфер.\n";
            return;
        }
        if (fee < transferFee_)
        {
            std::cout << "[ПОМИЛКА] Запропонована сума " << std::fixed <<
                std::setprecision(2)
                << fee << " € менша за мінімальну (" << transferFee_ <<
                " €).\n";
            return;
        }

        previousClub_ = contractDetails_.GetClubName();
        contractDetails_.SetClubName(newClub);
        listedForTransfer_ = false;

        std::cout << "[УСПІХ] " << GetName() << " проданий у клуб \"" << newClub
            << "\" за " << std::fixed << std::setprecision(2) << fee << " €.\n";
    }

    void ContractedPlayer::ExtendedContract(const std::string& newDate,
                                            double newSalary)
    {
        if (newSalary <= 0.0)
            throw std::invalid_argument("Зарплата повинна бути додатною.");

        const double oldSalary = contractDetails_.GetSalary();
        contractDetails_.ExtendContractDate(newDate);

        // змінюємо зарплату у відсотках, щоб зберегти єдину логіку
        const double perc = (oldSalary > 0.0)
                                ? ((newSalary - oldSalary) / oldSalary) * 100.0
                                : 100.0; // якщо стара була 0 → просто +100%
        contractDetails_.AdjustSalary(perc);

        std::cout << "[ІНФО] Контракт " << GetName() << " продовжено до "
            << newDate << ". Нова зарплата: "
            << std::fixed << std::setprecision(2) << newSalary << " €.\n";
    }

    void ContractedPlayer::TerminateContract(const std::string& reason)
    {
        contractDetails_.SetClubName("Без клубу (контракт розірвано)");
        std::cout << "[ПОПЕРЕДЖЕННЯ] Контракт " << GetName()
            << " розірвано. Причина: " << reason << "\n";
    }

    void ContractedPlayer::SendOnLoan(const std::string& otherClub,
                                      const std::string& endDate)
    {
        contractDetails_.SetOnLoan(endDate);
        std::cout << "[ІНФО] " << GetName() << " відправлений в оренду до "
            << otherClub << " до " << endDate << ".\n";
    }

    // === Інформаційні методи/поліморфізм ===
    void ContractedPlayer::ShowInfo() const
    {
        std::cout << "\n=== КОНТРАКТНИЙ ГРАВЕЦЬ ===\n";
        FieldPlayer::ShowInfo();
        contractDetails_.ShowDetails();

        if (listedForTransfer_)
        {
            std::cout << "Статус трансферу: У списку | Мін. сума: "
                << std::fixed << std::setprecision(2)
                << transferFee_ << " €\n";
            if (!transferConditions_.empty())
                std::cout << "Додаткові умови: " << transferConditions_ << "\n";
        }
        else
        {
            std::cout << "Статус трансферу: Не виставлений\n";
        }
    }

    void ContractedPlayer::CelebrateBirthday()
    {
        const int newAge = GetAge() + 1;
        SetAge(newAge);
        std::cout << "[ІНФО] З днем народження, " << GetName()
            << "! Тепер вам " << newAge << " років.\n";

        // Невелике вікове зниження після 30
        if (newAge > 30) UpdateMarketValue(-3.0);
    }

    double ContractedPlayer::CalculatePerformanceRating() const
    {
        // Проста формула за статистикою польового гравця
        // (ти можеш підкоригувати ваги під вимоги завдання)
        const double g = static_cast<double>(GetTotalGoals());
        const double a = static_cast<double>(GetTotalAssists());
        const double kp = static_cast<double>(GetKeyPasses());
        const double tk = static_cast<double>(GetTotalTackles());
        const double gp = static_cast<double>(GetTotalGames());

        if (gp <= 0.0) return 0.0;

        // ваги: гол 5, ассіст 3, ключовий пас 1, відбір 1.5 (усе на матч)
        const double perMatch = (5.0 * g + 3.0 * a + 1.0 * kp + 1.5 * tk) / gp;

        // Нормалізуємо у [0..10]
        const double rating = std::min(10.0, perMatch);
        return rating;
    }

    double ContractedPlayer::CalculateValue() const
    {
        const double baseValue = GetMarketValue();
        const double perfBonus = CalculatePerformanceRating() * 50'000.0;
        const double salaryImpact = contractDetails_.GetSalary() / 10'000.0;
        return baseValue + perfBonus + salaryImpact;
    }

    std::string ContractedPlayer::GetStatus() const
    {
        if (IsInjured()) return "Травмований гравець";
        if (contractDetails_.IsPlayerOnLoan()) return "В оренді";
        return "Активний гравець";
    }

    // === Серіалізація/десеріалізація ===
    std::string ContractedPlayer::Serialize() const
    {
        // Формуємо JSON-об’єкт вручну (без зовнішніх бібліотек)
        std::ostringstream ss;
        ss << "{"

            << Player::SerializeBase() << ","

            << "\"role\":\"ContractedPlayer\","
            << "\"position\":" << static_cast<int>(GetPosition()) << ","
            << "\"totalGames\":" << GetTotalGames() << ","
            << "\"totalGoals\":" << GetTotalGoals() << ","
            << "\"totalAssists\":" << GetTotalAssists() << ","
            << "\"totalShots\":" << GetTotalShots() << ","
            << "\"totalTackles\":" << GetTotalTackles() << ","
            << "\"keyPasses\":" << GetKeyPasses() << ","


            // контракт
            << "\"clubName\":\"" << contractDetails_.GetClubName() << "\","
            << "\"previousClub\":\"" << previousClub_ << "\","

            << "\"salary\":" << std::fixed << std::setprecision(2) <<
            contractDetails_.GetSalary() << ","
            << "\"contractUntil\":\"" << contractDetails_.GetContractUntil() <<
            "\","
            << "\"loaned\":" << (contractDetails_.IsPlayerOnLoan()
                                     ? "true"
                                     : "false");

        if (contractDetails_.IsPlayerOnLoan())
        {
            ss << "\"loanEndDate\":\"" << contractDetails_.GetLoanEndDate() <<
                "\",";
        }
        ss << ","

        // трансфер
        <<
        "\"listedForTransfer\":" << (listedForTransfer_
                                         ? "true"
                                         : "false") << ","
            << "\"transferFee\":" << std::fixed << std::setprecision(2) <<
            transferFee_ << ","
            << "\"transferConditions\":\"" << transferConditions_ << "\""
            << "}";
        return ss.str();
    }

    void ContractedPlayer::Deserialize(const std::string& data)
    {
        DeserializeBase(data);

        auto findString = [&](const std::string& key) -> std::string
        {
            const std::string pat = "\"" + key + "\":\"";
            const auto pos = data.find(pat);
            if (pos == std::string::npos) return {};
            const auto start = pos + pat.size();
            const auto end = data.find('"', start);
            if (end == std::string::npos) return {};
            return data.substr(start, end - start);
        };
        auto findNumber = [&](const std::string& key) -> double
        {
            const std::string pat = "\"" + key + "\":";
            const auto pos = data.find(pat);
            if (pos == std::string::npos) return 0.0;
            const auto start = pos + pat.size();
            size_t end = start;
            while (end < data.size() && (isdigit(data[end]) || data[end] == '.'
                || data[end] == '-'))
                ++end;
            return std::stod(data.substr(start, end - start));
        };
        auto findBool = [&](const std::string& key) -> bool
        {
            const std::string pat = "\"" + key + "\":";
            const auto pos = data.find(pat);
            if (pos == std::string::npos) return false;
            const auto start = pos + pat.size();
            if (data.compare(start, 4, "true") == 0) return true;
            if (data.compare(start, 5, "false") == 0) return false;
            return false;
        };

        {
            const std::string k = "\"position\":";
            auto pos = data.find(k);
            if (pos != std::string::npos)
            {
                size_t i = pos + k.size();
                while (i < data.size() && (data[i] == ' ')) ++i;
                int p = 0;
                try { p = std::stoi(data.substr(i)); }
                catch (...) { p = 0; }
                SetPosition(static_cast<Position>(p));
            }
        }

        const int games = static_cast<int>(findNumber("totalGames"));
        const int goals = static_cast<int>(findNumber("totalGoals"));
        const int assists = static_cast<int>(findNumber("totalAssists"));
        const int shots = static_cast<int>(findNumber("totalShots"));
        const int tackles = static_cast<int>(findNumber("totalTackles"));
        const int kpasses = static_cast<int>(findNumber("keyPasses"));

        for (int i = 0; i < games; ++i) RegisterMatchPlayed();
        if (goals > 0 || assists > 0 || shots > 0)
            UpdateAttackingStats(
                goals, assists, shots);
        if (tackles > 0) UpdateDefensiveStats(tackles);
        for (int i = 0; i < kpasses; ++i) RegisterKeyPass();

        const std::string club = findString("clubName");
        const double sal = findNumber("salary");
        const std::string until = findString("contractUntil");
        const bool loan = findBool("loaned");
        const std::string loanEnd = findString("loanEndDate");

        if (!club.empty()) contractDetails_.SetClubName(club);
        if (sal >= 0.0) contractDetails_.SetSalary(sal);
        if (!until.empty()) contractDetails_.SetContractUntil(until);
        if (loan)
        {

            try
            {
                if (!loanEnd.empty())
                {
                    contractDetails_.SetOnLoan(loanEnd);
                }
                else
                {
                    // fallback: можна не вмикати оренду без дати, або
                    // обрати розумну дефолтну (наприклад, кінець поточного контракту)
                    // contractDetails_.SetOnLoan(contractDetails_.GetContractUntil());
                }
            }
            catch (const std::exception& e)
            {
                std::cout << "[ПОПЕРЕДЖЕННЯ] Некоректна loanEndDate у даних: " << e.what() << "\n";
                // Безпечний варіант: залишити без оренди або повернути з оренди
                // contractDetails_.ReturnFromLoan();
            }
        }
        else
        {
            contractDetails_.ReturnFromLoan();
        }

        listedForTransfer_ = findBool("listedForTransfer");
        transferFee_ = findNumber("transferFee");
        const std::string cond = findString("transferConditions");
        if (!cond.empty()) transferConditions_ = cond;
        const std::string prev = findString("previousClub");
        if (!prev.empty()) previousClub_ = prev;
    }
} // namespace FootballManagement
