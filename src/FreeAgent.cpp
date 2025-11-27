#include "../include/FreeAgent.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <cctype>

namespace FootballManagement
{
    FreeAgent::FreeAgent()
        : FieldPlayer(),
          expectedSalary_(0.0),
          lastClub_("Невідомо"),
          monthsWithoutClub_(0),
          availableForNegotiation_(true)
    {
    }

    FreeAgent::FreeAgent(const std::string& name, int age,
                         const std::string& nationality,
                         const std::string& origin,
                         double height, double weight, double marketValue,
                         Position position, double expectedSalary,
                         const std::string& lastClub)
        : FieldPlayer(name, age, nationality, origin, height, weight,
                      marketValue, position),
          expectedSalary_(expectedSalary),
          lastClub_(lastClub),
          monthsWithoutClub_(0),
          availableForNegotiation_(true)
    {
        if (expectedSalary_ < 0.0)
            throw std::invalid_argument(
                "Очікувана зарплата не може бути від’ємною.");
    }

    FreeAgent::FreeAgent(const FreeAgent& other)
        : FieldPlayer(other),
          expectedSalary_(other.expectedSalary_),
          lastClub_(other.lastClub_),
          monthsWithoutClub_(other.monthsWithoutClub_),
          availableForNegotiation_(other.availableForNegotiation_)
    {
    }

    FreeAgent::FreeAgent(FreeAgent&& other) noexcept
        : FieldPlayer(std::move(other)),
          expectedSalary_(other.expectedSalary_),
          lastClub_(std::move(other.lastClub_)),
          monthsWithoutClub_(other.monthsWithoutClub_),
          availableForNegotiation_(other.availableForNegotiation_)
    {
        other.expectedSalary_ = 0.0;
        other.monthsWithoutClub_ = 0;
        other.availableForNegotiation_ = true;
    }

    FreeAgent& FreeAgent::operator=(const FreeAgent& other)
    {
        if (this != &other)
        {
            FieldPlayer::operator=(other);
            expectedSalary_ = other.expectedSalary_;
            lastClub_ = other.lastClub_;
            monthsWithoutClub_ = other.monthsWithoutClub_;
            availableForNegotiation_ = other.availableForNegotiation_;
        }
        return *this;
    }

    FreeAgent& FreeAgent::operator=(FreeAgent&& other) noexcept
    {
        if (this != &other)
        {
            FieldPlayer::operator=(std::move(other));
            expectedSalary_ = other.expectedSalary_;
            lastClub_ = std::move(other.lastClub_);
            monthsWithoutClub_ = other.monthsWithoutClub_;
            availableForNegotiation_ = other.availableForNegotiation_;

            other.expectedSalary_ = 0.0;
            other.monthsWithoutClub_ = 0;
            other.availableForNegotiation_ = true;
        }
        return *this;
    }

    FreeAgent::~FreeAgent() noexcept
    {
        std::cout << "[ДЕБАГ] Вільного агента \"" << GetName() <<
            "\" знищено.\n";
    }

    double FreeAgent::GetExpectedSalary() const { return expectedSalary_; }
    std::string FreeAgent::GetLastClub() const { return lastClub_; }

    bool FreeAgent::IsAvailableForNegotiation() const
    {
        return availableForNegotiation_;
    }

    int FreeAgent::GetMonthWithoutClub() const { return monthsWithoutClub_; }

    void FreeAgent::SetLastClub(const std::string& lastClub)
    {
        lastClub_ = lastClub;
    }

    void FreeAgent::SetMonthWithoutClub(int months)
    {
        if (months < 0)
            throw std::invalid_argument(
                "Кількість місяців без клубу не може бути від’ємною.");
        monthsWithoutClub_ = months;
    }

    void FreeAgent::SetAvailability(bool isAvailable)
    {
        availableForNegotiation_ = isAvailable;
        std::cout << "[ІНФО] " << GetName()
            << (isAvailable ? " відкрив " : " закрив ")
            << "переговори.\n";
    }

    bool FreeAgent::NegotiateOffer(double offer)
    {
        if (!availableForNegotiation_)
        {
            std::cout << "[ПОМИЛКА] " << GetName() <<
                " не веде переговорів зараз.\n";
            return false;
        }

        if (offer >= expectedSalary_)
        {
            std::cout << "[УСПІХ] " << GetName()
                << " прийняв пропозицію із зарплатою "
                << std::fixed << std::setprecision(2)
                << offer << " €.\n";
            availableForNegotiation_ = false;
            return true;
        }

        std::cout << "[ІНФО] " << GetName()
            << " відхилив пропозицію " << std::fixed << std::setprecision(2)
            << offer << " € (очікує "
            << expectedSalary_ << " €).\n";
        return false;
    }

    void FreeAgent::IncreaseExpectations(double percentage)
    {
        if (percentage <= 0.0) return;
        expectedSalary_ *= (1.0 + percentage / 100.0);
        std::cout << "[ІНФО] Очікування " << GetName()
            << " зросли на " << percentage << "%. Нова зарплата: "
            << std::fixed << std::setprecision(2)
            << expectedSalary_ << " €.\n";
    }

    void FreeAgent::DecreaseExpectations(double percentage)
    {
        if (percentage <= 0.0) return;
        expectedSalary_ *= (1.0 - percentage / 100.0);
        if (expectedSalary_ < 0.0) expectedSalary_ = 0.0;
        std::cout << "[ІНФО] Очікування " << GetName()
            << " знижено на " << percentage << "%. Нова зарплата: "
            << std::fixed << std::setprecision(2)
            << expectedSalary_ << " €.\n";
    }

    void FreeAgent::AcceptContract(const std::string& club)
    {
        SetAvailability(false);
        lastClub_ = club;
        monthsWithoutClub_ = 0;
        std::cout << "[УСПІХ] " << GetName()
            << " підписав контракт із клубом \"" << club << "\".\n";
    }

    bool FreeAgent::IsBargain() const
    {
        return (CalculateValue() > expectedSalary_ * 1.3) && (GetAge() <= 30);
    }

    void FreeAgent::ShowInfo() const
    {
        std::cout << "\n=== ВІЛЬНИЙ АГЕНТ ===\n";
        FieldPlayer::ShowInfo();
        std::cout << "ID: " << GetPlayerId() << "\n";
        std::cout << "Останній клуб: " << lastClub_
            << " | Місяців без клубу: " << monthsWithoutClub_ << "\n";
        std::cout << "Очікувана зарплата: " << std::fixed <<
            std::setprecision(2)
            << expectedSalary_ << " € | Доступний до переговорів: "
            << (availableForNegotiation_ ? "Так" : "Ні") << "\n";
    }

    void FreeAgent::CelebrateBirthday()
    {
        const int newAge = GetAge() + 1;
        SetAge(newAge);
        std::cout << "[ІНФО] З днем народження, " << GetName()
            << "! Тепер вам " << newAge << " років.\n";
        IncreaseExpectations(3.0);
    }

    double FreeAgent::CalculatePerformanceRating() const
    {
        const double g = static_cast<double>(GetTotalGoals());
        const double a = static_cast<double>(GetTotalAssists());
        const double kp = static_cast<double>(GetKeyPasses());
        const double tk = static_cast<double>(GetTotalTackles());
        const double gp = static_cast<double>(GetTotalGames());

        if (gp <= 0.0)
        {
            double base = 5.0 - monthsWithoutClub_ * 0.3;
            if (base < 0.0) base = 0.0;
            return base;
        }

        double perMatch = (5.0 * g + 3.0 * a + 1.0 * kp + 1.5 * tk) / gp;
        perMatch -= monthsWithoutClub_ * 0.2;
        if (perMatch < 0.0) perMatch = 0.0;
        if (perMatch > 10.0) perMatch = 10.0;
        return perMatch;
    }

    double FreeAgent::CalculateValue() const
    {
        double discount = 1.0 - monthsWithoutClub_ * 0.05;
        if (discount < 0.5) discount = 0.5;
        const double perfBonus = CalculatePerformanceRating() * 40'000.0;
        return GetMarketValue() * discount + perfBonus;
    }

    std::string FreeAgent::GetStatus() const
    {
        if (IsInjured()) return "Травмований вільний агент";
        return availableForNegotiation_
                   ? "Вільний агент (активний)"
                   : "Контракт підписано";
    }

    std::string FreeAgent::Serialize() const
    {
        std::ostringstream ss;
        ss << "{"
            << SerializeBase() << ","
            << "\"position\":" << static_cast<int>(GetPosition()) << ","
            << "\"totalGames\":" << GetTotalGames() << ","
            << "\"totalGoals\":" << GetTotalGoals() << ","
            << "\"totalAssists\":" << GetTotalAssists() << ","
            << "\"totalShots\":" << GetTotalShots() << ","
            << "\"totalTackles\":" << GetTotalTackles() << ","
            << "\"keyPasses\":" << GetKeyPasses() << ","

            << "\"role\":\"FreeAgent\","
            << "\"expectedSalary\":" << std::fixed << std::setprecision(2) <<
            expectedSalary_ << ","
            << "\"lastClub\":\"" << lastClub_ << "\","
            << "\"monthsWithoutClub\":" << monthsWithoutClub_ << ","
            << "\"available\":" << (availableForNegotiation_ ? "true" : "false")
            << "}";
        return ss.str();
    }

    void FreeAgent::Deserialize(const std::string& data)
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
            while (end < data.size() && (isdigit(
                    static_cast<unsigned char>(data[end])) || data[end] == '.'
                ||
                data[end] == '-'))
                ++end;
            try { return std::stod(data.substr(start, end - start)); }
            catch (...) { return 0.0; }
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
                while (i < data.size() && data[i] == ' ') ++i;
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

        const std::string club = findString("lastClub");
        if (!club.empty()) lastClub_ = club;

        expectedSalary_ = findNumber("expectedSalary");
        monthsWithoutClub_ = static_cast<int>(findNumber("monthsWithoutClub"));
        availableForNegotiation_ = findBool("available");
    }
}
