#include "../include/FieldPlayer.h"

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <utility>

namespace
{
    // Локальна утиліта для красивого виводу позиції
    std::string PositionToString(FootballManagement::Position p)
    {
        using FootballManagement::Position;
        switch (p)
        {
        case Position::Goalkeeper: return "Воротар";
        case Position::Defender: return "Захисник";
        case Position::Midfielder: return "Півзахисник";
        case Position::Forward: return "Нападник";
        }
        return "Невідомо";
    }
}

namespace FootballManagement
{
    // Конструктори/деструктор
    FieldPlayer::FieldPlayer()
        : Player(),
          totalGames_(0),
          totalGoals_(0),
          totalAssists_(0),
          totalShots_(0),
          totalTackles_(0),
          keyPasses_(0),
          position_(Position::Forward)
    {
    }

    FieldPlayer::FieldPlayer(const std::string& name, int age,
                             const std::string& nationality,
                             const std::string& origin,
                             double height, double weight,
                             double marketValue, Position position)
        : Player(name, age, nationality, origin, height, weight, marketValue),
          totalGames_(0),
          totalGoals_(0),
          totalAssists_(0),
          totalShots_(0),
          totalTackles_(0),
          keyPasses_(0),
          position_(position)
    {
    }

    FieldPlayer::FieldPlayer(const FieldPlayer& other)
        : Player(other),
          totalGames_(other.totalGames_),
          totalGoals_(other.totalGoals_),
          totalAssists_(other.totalAssists_),
          totalShots_(other.totalShots_),
          totalTackles_(other.totalTackles_),
          keyPasses_(other.keyPasses_),
          position_(other.position_)
    {
    }

    FieldPlayer::FieldPlayer(FieldPlayer&& other) noexcept
        : Player(std::move(other)),
          totalGames_(other.totalGames_),
          totalGoals_(other.totalGoals_),
          totalAssists_(other.totalAssists_),
          totalShots_(other.totalShots_),
          totalTackles_(other.totalTackles_),
          keyPasses_(other.keyPasses_),
          position_(other.position_)
    {
        other.totalGames_ = 0;
        other.totalGoals_ = 0;
        other.totalAssists_ = 0;
        other.totalShots_ = 0;
        other.totalTackles_ = 0;
        other.keyPasses_ = 0;
    }

    FieldPlayer& FieldPlayer::operator=(const FieldPlayer& other)
    {
        if (this != &other)
        {
            Player::operator=(other);
            totalGames_ = other.totalGames_;
            totalGoals_ = other.totalGoals_;
            totalAssists_ = other.totalAssists_;
            totalShots_ = other.totalShots_;
            totalTackles_ = other.totalTackles_;
            keyPasses_ = other.keyPasses_;
            position_ = other.position_;
        }
        return *this;
    }

    FieldPlayer& FieldPlayer::operator=(FieldPlayer&& other) noexcept
    {
        if (this != &other)
        {
            Player::operator=(std::move(other));
            totalGames_ = other.totalGames_;
            totalGoals_ = other.totalGoals_;
            totalAssists_ = other.totalAssists_;
            totalShots_ = other.totalShots_;
            totalTackles_ = other.totalTackles_;
            keyPasses_ = other.keyPasses_;
            position_ = other.position_;

            other.totalGames_ = 0;
            other.totalGoals_ = 0;
            other.totalAssists_ = 0;
            other.totalShots_ = 0;
            other.totalTackles_ = 0;
            other.keyPasses_ = 0;
        }
        return *this;
    }

    FieldPlayer::~FieldPlayer()
    {
        std::cout << "[ДЕБАГ] Польового гравця \"" << GetName()
            << "\" знищено.\n";
    }

    // Властивості
    int FieldPlayer::GetTotalGames() const { return totalGames_; }
    int FieldPlayer::GetTotalGoals() const { return totalGoals_; }
    int FieldPlayer::GetTotalAssists() const { return totalAssists_; }
    int FieldPlayer::GetTotalShots() const { return totalShots_; }
    int FieldPlayer::GetTotalTackles() const { return totalTackles_; }
    int FieldPlayer::GetKeyPasses() const { return keyPasses_; }
    Position FieldPlayer::GetPosition() const { return position_; }

    void FieldPlayer::SetPosition(Position position)
    {
        position_ = position;
    }

    // Статистика
    void FieldPlayer::UpdateAttackingStats(int goals, int assists, int shots)
    {
        if (goals < 0 || assists < 0 || shots < 0)
            throw std::invalid_argument(
                "Помилка: статистика не може бути від’ємною.");

        totalGoals_ += goals;
        totalAssists_ += assists;
        totalShots_ += shots;
    }

    void FieldPlayer::UpdateDefensiveStats(int tackles)
    {
        if (tackles < 0)
            throw std::invalid_argument(
                "Помилка: кількість відборів не може бути від’ємною.");

        totalTackles_ += tackles;
    }

    void FieldPlayer::RegisterKeyPass()
    {
        ++keyPasses_;
    }

    double FieldPlayer::CalculateConversionRate() const
    {
        if (totalShots_ == 0) return 0.0;
        return (static_cast<double>(totalGoals_) / totalShots_) * 100.0;
    }

    void FieldPlayer::RegisterMatchPlayed()
    {
        ++totalGames_;
    }

    void FieldPlayer::ResetSeasonStats()
    {
        totalGames_ = 0;
        totalGoals_ = 0;
        totalAssists_ = 0;
        totalShots_ = 0;
        totalTackles_ = 0;
        keyPasses_ = 0;

        std::cout << "[ІНФО] Статистику сезону для гравця \"" << GetName()
            << "\" обнулено.\n";
    }

    // Вивід
    void FieldPlayer::ShowInfo() const
    {
        std::cout << "\n=== Інформація про польового гравця ===\n";
        std::cout << "Ім’я: " << GetName()
            << " | Вік: " << GetAge()
            << " | Позиція: " << PositionToString(position_) << "\n";

        std::cout << "Матчів: " << totalGames_
            << " | Голів: " << totalGoals_
            << " | Асистів: " << totalAssists_ << "\n";

        std::cout << "Удари: " << totalShots_
            << " | Відбори: " << totalTackles_
            << " | Ключові паси: " << keyPasses_ << "\n";

        std::cout << "Конверсія ударів: " << std::fixed << std::setprecision(2)
            << CalculateConversionRate() << "%\n";
    }

    void FieldPlayer::CelebrateBirthday()
    {
        const int newAge = GetAge() + 1;
        SetAge(newAge); // потребує наявності сетера в Player

        std::cout << "[ІНФО] З днем народження, " << GetName()
            << "! Тепер вам " << newAge << " років. "
            << "Бажаємо нових перемог!\n";
    }
} // namespace FootballManagement
