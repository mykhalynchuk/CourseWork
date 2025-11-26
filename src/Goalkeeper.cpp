#include "../include/Goalkeeper.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <cctype>

namespace FootballManagement
{
    // === Конструктори/оператори ===

    Goalkeeper::Goalkeeper()
        : Player(),
          matchesPlayed_(0),
          cleanSheets_(0),
          savesTotal_(0),
          goalsConceded_(0),
          penaltiesSaved_(0)
    {
    }

    Goalkeeper::Goalkeeper(const std::string& name, int age,
                           const std::string& nationality,
                           const std::string& origin,
                           double height, double weight, double marketValue)
        : Player(name, age, nationality, origin, height, weight, marketValue),
          matchesPlayed_(0),
          cleanSheets_(0),
          savesTotal_(0),
          goalsConceded_(0),
          penaltiesSaved_(0)
    {
    }

    Goalkeeper::Goalkeeper(const Goalkeeper& other)
        : Player(other),
          matchesPlayed_(other.matchesPlayed_),
          cleanSheets_(other.cleanSheets_),
          savesTotal_(other.savesTotal_),
          goalsConceded_(other.goalsConceded_),
          penaltiesSaved_(other.penaltiesSaved_)
    {
    }

    Goalkeeper::Goalkeeper(Goalkeeper&& other) noexcept
        : Player(std::move(other)),
          matchesPlayed_(other.matchesPlayed_),
          cleanSheets_(other.cleanSheets_),
          savesTotal_(other.savesTotal_),
          goalsConceded_(other.goalsConceded_),
          penaltiesSaved_(other.penaltiesSaved_)
    {
        other.matchesPlayed_ = 0;
        other.cleanSheets_ = 0;
        other.savesTotal_ = 0;
        other.goalsConceded_ = 0;
        other.penaltiesSaved_ = 0;
    }

    Goalkeeper& Goalkeeper::operator=(const Goalkeeper& other)
    {
        if (this != &other)
        {
            Player::operator=(other);
            matchesPlayed_ = other.matchesPlayed_;
            cleanSheets_ = other.cleanSheets_;
            savesTotal_ = other.savesTotal_;
            goalsConceded_ = other.goalsConceded_;
            penaltiesSaved_ = other.penaltiesSaved_;
        }
        return *this;
    }

    Goalkeeper& Goalkeeper::operator=(Goalkeeper&& other) noexcept
    {
        if (this != &other)
        {
            Player::operator=(std::move(other));
            matchesPlayed_ = other.matchesPlayed_;
            cleanSheets_ = other.cleanSheets_;
            savesTotal_ = other.savesTotal_;
            goalsConceded_ = other.goalsConceded_;
            penaltiesSaved_ = other.penaltiesSaved_;

            other.matchesPlayed_ = 0;
            other.cleanSheets_ = 0;
            other.savesTotal_ = 0;
            other.goalsConceded_ = 0;
            other.penaltiesSaved_ = 0;
        }
        return *this;
    }

    Goalkeeper::~Goalkeeper() noexcept
    {
        std::cout << "[ДЕБАГ] Воротаря \"" << GetName() << "\" знищено.\n";
    }

    // === Властивості ===

    int Goalkeeper::GetMatchesPlayed() const { return matchesPlayed_; }
    int Goalkeeper::GetCleanSheets() const { return cleanSheets_; }
    int Goalkeeper::GetSavesTotal() const { return savesTotal_; }
    int Goalkeeper::GetGoalsConceded() const { return goalsConceded_; }
    int Goalkeeper::GetPenaltiesSaved() const { return penaltiesSaved_; }

    // === Операції зі статистикою ===

    void Goalkeeper::UpdateMatchStats(int goalsAgainst, int saves)
    {
        if (goalsAgainst < 0 || saves < 0)
            throw std::invalid_argument(
                "Помилка: статистика не може бути від’ємною.");

        ++matchesPlayed_;
        goalsConceded_ += goalsAgainst;
        savesTotal_ += saves;
        if (goalsAgainst == 0) RegisterCleanSheet();
    }

    void Goalkeeper::RegisterCleanSheet()
    {
        ++cleanSheets_;
    }

    void Goalkeeper::RegisterPenaltySave()
    {
        ++penaltiesSaved_;
    }

    double Goalkeeper::CalculateSavePercentage() const
    {
        const int faced = savesTotal_ + goalsConceded_;
        if (faced == 0) return 0.0;
        return (static_cast<double>(savesTotal_) / faced) * 100.0;
    }

    bool Goalkeeper::IsVeteran() const
    {
        return GetAge() >= 35;
    }

    void Goalkeeper::ResetSeasonStats()
    {
        matchesPlayed_ = 0;
        cleanSheets_ = 0;
        savesTotal_ = 0;
        goalsConceded_ = 0;
        penaltiesSaved_ = 0;

        std::cout << "[ІНФО] Статистику воротаря \"" << GetName()
            << "\" обнулено.\n";
    }

    // === Поліморфні методи Player ===

    void Goalkeeper::ShowInfo() const
    {
        std::cout << "\n=== ІНФОРМАЦІЯ ПРО ВОРОТАРЯ ===\n";
        std::cout << "ID: " << GetPlayerId() << "\n";
        std::cout << "Ім’я: " << GetName()
            << " | Вік: " << GetAge()
            << " | Статус: " << GetStatus() << "\n";
        std::cout << "Матчів: " << matchesPlayed_
            << " | Сухих матчів: " << cleanSheets_ << "\n";
        std::cout << "Сейви: " << savesTotal_
            << " | Пропущено голів: " << goalsConceded_
            << " | Відбиті пенальті: " << penaltiesSaved_ << "\n";
        std::cout << "Відсоток сейвів: " << std::fixed << std::setprecision(2)
            << CalculateSavePercentage() << "%\n";
    }

    void Goalkeeper::CelebrateBirthday()
    {
        const int newAge = GetAge() + 1;
        SetAge(newAge); // сетер ми додали у Player
        std::cout << "[ІНФО] З днем народження, воротарю " << GetName()
            << "! Тепер вам " << newAge << " років.\n";
    }

    double Goalkeeper::CalculatePerformanceRating() const
    {
        if (matchesPlayed_ == 0) return 0.0;

        // ваги: clean sheet = 4, пенальті = 3, сейви (у відсотках) / 10
        const double saveFactor = CalculateSavePercentage() / 10.0;
        const double goalsPerGame = static_cast<double>(goalsConceded_) /
            matchesPlayed_;
        double rating = (cleanSheets_ * 4.0) + (penaltiesSaved_ * 3.0) +
            saveFactor
            - (goalsPerGame * 2.0);
        if (rating < 0.0) rating = 0.0;
        if (rating > 10.0) rating = 10.0;
        return rating;
    }

    double Goalkeeper::CalculateValue() const
    {
        const double base = GetMarketValue();
        const double perf = CalculatePerformanceRating() * 100'000.0;
        const double csBonus = cleanSheets_ * 300'000.0;
        return base + perf + csBonus;
    }

    std::string Goalkeeper::GetStatus() const
    {
        return IsInjured() ? "Травмований воротар" : "Активний воротар";
    }

    // === Серіалізація/десеріалізація ===

    std::string Goalkeeper::Serialize() const
    {
        // JSON-подібний рядок (без сторонніх бібліотек)
        std::ostringstream ss;
        ss << "{"
            << SerializeBase() << "," // базові поля Player
            << "\"role\":\"Goalkeeper\"," // корисна мітка типу
            << "\"matchesPlayed\":" << matchesPlayed_ << ","
            << "\"cleanSheets\":" << cleanSheets_ << ","
            << "\"savesTotal\":" << savesTotal_ << ","
            << "\"goalsConceded\":" << goalsConceded_ << ","
            << "\"penaltiesSaved\":" << penaltiesSaved_
            << "}";
        return ss.str();
    }

    void Goalkeeper::Deserialize(const std::string& data)
    {
        // Базові поля Player
        DeserializeBase(data);

        // Прості пошуки значень (можна замінити на твій парсер з Utils)
        auto findNumber = [&](const std::string& key, int def = 0) -> int
        {
            const std::string pat = "\"" + key + "\":";
            const auto pos = data.find(pat);
            if (pos == std::string::npos) return def;
            size_t i = pos + pat.size();
            // пропустити пробіли/знаки
            while (i < data.size() && (data[i] == ' ')) ++i;
            size_t j = i;
            while (j < data.size() && (std::isdigit(
                static_cast<unsigned char>(data[j])) || data[j] == '-'))
                ++j;
            try { return std::stoi(data.substr(i, j - i)); }
            catch (...) { return def; }
        };

        matchesPlayed_ = findNumber("matchesPlayed", 0);
        cleanSheets_ = findNumber("cleanSheets", 0);
        savesTotal_ = findNumber("savesTotal", 0);
        goalsConceded_ = findNumber("goalsConceded", 0);
        penaltiesSaved_ = findNumber("penaltiesSaved", 0);
    }
} // namespace FootballManagement
