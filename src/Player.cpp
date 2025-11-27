#include "../include/Player.h"

#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace FootballManagement
{
    Player::Player()
        : playerId_(0),
          name_("Невідомо"),
          age_(0),
          nationality_("Невідомо"),
          origin_("Невідомий"),
          height_(0.0),
          weight_(0.0),
          marketValue_(0.0),
          injured_(false)
    {
    }

    Player::Player(const std::string& name, int age,
                   const std::string& nationality, const std::string& origin,
                   double height, double weight, double marketValue)
        : playerId_(0),
          name_(name),
          age_(age),
          nationality_(nationality),
          origin_(origin.empty() ? "Невідомий" : origin),
          height_(height),
          weight_(weight),
          marketValue_(marketValue),
          injured_(false)
    {
        if (age <= 0) throw std::invalid_argument("Вік має бути додатним.");
        if (height <= 0.0)
            throw std::invalid_argument(
                "Зріст має бути додатним.");
        if (weight <= 0.0)
            throw std::invalid_argument(
                "Вага має бути додатною.");
        if (marketValue < 0.0)
            throw std::invalid_argument(
                "Ринкова вартість не може бути від'ємною.");
    }

    Player::Player(const Player& other) = default;

    Player::Player(Player&& other) noexcept
        : playerId_(other.playerId_),
          name_(std::move(other.name_)),
          age_(other.age_),
          nationality_(std::move(other.nationality_)),
          origin_(std::move(other.origin_)),
          height_(other.height_),
          weight_(other.weight_),
          marketValue_(other.marketValue_),
          injured_(other.injured_),
          injuryHistory_(std::move(other.injuryHistory_))
    {
        other.playerId_ = 0;
        other.injured_ = false;
    }

    Player& Player::operator=(const Player& other)
    {
        if (this != &other)
        {
            playerId_ = other.playerId_;
            name_ = other.name_;
            age_ = other.age_;
            nationality_ = other.nationality_;
            origin_ = other.origin_;
            height_ = other.height_;
            weight_ = other.weight_;
            marketValue_ = other.marketValue_;
            injured_ = other.injured_;
            injuryHistory_ = other.injuryHistory_;
        }
        return *this;
    }

    Player& Player::operator=(Player&& other) noexcept
    {
        if (this != &other)
        {
            playerId_ = other.playerId_;
            name_ = std::move(other.name_);
            age_ = other.age_;
            nationality_ = std::move(other.nationality_);
            origin_ = std::move(other.origin_);
            height_ = other.height_;
            weight_ = other.weight_;
            marketValue_ = other.marketValue_;
            injured_ = other.injured_;
            injuryHistory_ = std::move(other.injuryHistory_);

            other.playerId_ = 0;
            other.injured_ = false;
        }
        return *this;
    }

    Player::~Player() noexcept
    {
        std::cout << "[ДЕБАГ] Гравець \"" << name_
            << "\" (ID: " << playerId_ << ") видалений.\n";
    }

    int Player::GetPlayerId() const { return playerId_; }
    std::string Player::GetName() const { return name_; }
    int Player::GetAge() const { return age_; }
    std::string Player::GetNationality() const { return nationality_; }
    std::string Player::GetOrigin() const { return origin_; }
    double Player::GetHeight() const { return height_; }
    double Player::GetWeight() const { return weight_; }
    double Player::GetMarketValue() const { return marketValue_; }
    bool Player::IsInjured() const { return injured_; }

    const std::vector<Injury>& Player::GetInjuryHistory() const
    {
        return injuryHistory_;
    }

    void Player::SetPlayerId(int playerId)
    {
        if (playerId <= 0)
            throw std::invalid_argument(
                "Ідентифікатор гравця має бути додатним.");
        playerId_ = playerId;
    }

    void Player::SetName(const std::string& name)
    {
        if (name.empty())
            throw std::invalid_argument("Ім'я гравця не може бути порожнім.");
        name_ = name;
    }

    void Player::SetAge(int age)
    {
        if (age <= 0)
            throw std::invalid_argument("Вік повинен бути додатним.");
        age_ = age;
    }


    void Player::SetNationality(const std::string& nationality)
    {
        if (nationality.empty())
            throw std::invalid_argument(
                "Національність не може бути порожньою.");
        nationality_ = nationality;
    }

    void Player::SetOrigin(const std::string& origin)
    {
        origin_ = origin.empty() ? "Невідомий" : origin;
    }

    void Player::SetHeight(double height)
    {
        if (height <= 0.0)
            throw std::invalid_argument("Зріст має бути додатним.");
        height_ = height;
    }

    void Player::SetWeight(double weight)
    {
        if (weight <= 0.0)
            throw std::invalid_argument("Вага має бути додатною.");
        weight_ = weight;
    }

    void Player::SetMarketValue(double value)
    {
        if (value < 0.0)
            throw std::invalid_argument(
                "Ринкова вартість не може бути від'ємною.");
        marketValue_ = value;
    }

    void Player::ReportInjury(const std::string& type, int recoveryDays)
    {
        if (type.empty())
            throw std::invalid_argument("Тип травми не може бути порожнім.");
        if (recoveryDays <= 0)
            throw std::invalid_argument("Дні відновлення мають бути > 0.");

        injured_ = true;

        Injury inj;
        inj.injuryType = type;
        inj.recoveryDays = recoveryDays;
        inj.dateOccurred = "сьогодні";

        injuryHistory_.push_back(std::move(inj));

        std::cout << "Гравець \"" << name_ << "\" отримав травму: " << type
            << ". Орієнтовне відновлення: " << recoveryDays << " днів.\n";
    }

    void Player::ReturnToFitness()
    {
        injured_ = false;
        std::cout << "Гравець \"" << name_ << "\" відновився після травми.\n";
    }

    void Player::UpdateMarketValue(double percentageChange)
    {
        const double factor = 1.0 + (percentageChange / 100.0);
        marketValue_ *= factor;
        if (marketValue_ < 0.0) marketValue_ = 0.0;
    }

    std::string Player::SerializeBase() const
    {
        std::ostringstream ss;
        ss << "\"id\":" << playerId_ << ','
            << "\"name\":\"" << name_ << "\","
            << "\"age\":" << age_ << ','
            << "\"nationality\":\"" << nationality_ << "\","
            << "\"origin\":\"" << origin_ << "\","
            << "\"height\":" << height_ << ','
            << "\"weight\":" << weight_ << ','
            << "\"marketValue\":" << marketValue_ << ','
            << "\"injured\":" << (injured_ ? "true" : "false");
        return ss.str();
    }

    void Player::DeserializeBase(const std::string& json)
    {
        std::regex r("\"(.*?)\"\\s*:\\s*\"?(.*?)\"?(,|})");
        auto begin = std::sregex_iterator(json.begin(), json.end(), r);
        auto end = std::sregex_iterator();

        for (auto it = begin; it != end; ++it)
        {
            const std::string key = (*it)[1].str();
            const std::string value = (*it)[2].str();

            if (key == "id") playerId_ = std::stoi(value);
            else if (key == "name") name_ = value;
            else if (key == "age") age_ = std::stoi(value);
            else if (key == "nationality") nationality_ = value;
            else if (key == "origin") origin_ = value;
            else if (key == "height") height_ = std::stod(value);
            else if (key == "weight") weight_ = std::stod(value);
            else if (key == "marketValue") marketValue_ = std::stod(value);
            else if (key == "injured") injured_ = (value == "true");
        }
    }
}
