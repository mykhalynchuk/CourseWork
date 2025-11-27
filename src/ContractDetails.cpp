#include "../include/ContractDetails.h"

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <ctime>

namespace FootballManagement
{
    bool ContractDetails::ValidateIsoDate(const std::string& s)
    {
        if (s.size() != 10) return false;
        // YYYY-MM-DD
        if (s[4] != '-' || s[7] != '-') return false;
        for (size_t i : {0u, 1u, 2u, 3u, 5u, 6u, 8u, 9u})
            if (s[i] < '0' || s[i] > '9') return false;

        int y = std::stoi(s.substr(0, 4));
        int m = std::stoi(s.substr(5, 2));
        int d = std::stoi(s.substr(8, 2));
        if (y < 1900 || y > 2100) return false;
        if (m < 1 || m > 12) return false;
        if (d < 1 || d > 31) return false;
        return true;
    }

    ContractDetails::ContractDetails()
        : clubName_("Невідомо"),
          salary_(0.0),
          contractUntil_("Невідомо"),
          isLoaned_(false),
          loanEndDate_("")
    {
    }

    ContractDetails::ContractDetails(const std::string& clubName,
                                     double salary,
                                     const std::string& contractUntil)
        : clubName_(clubName),
          salary_(salary),
          contractUntil_(contractUntil),
          isLoaned_(false),
          loanEndDate_("")
    {
        if (clubName_.empty())
            throw std::invalid_argument("Назва клубу не може бути порожньою.");
        if (salary_ < 0.0)
            throw std::invalid_argument("Зарплата не може бути від’ємною.");
        if (!ValidateIsoDate(contractUntil_))
            throw std::invalid_argument(
                "Невірний формат дати (очікується YYYY-MM-DD).");
    }

    ContractDetails::ContractDetails(const ContractDetails& other) = default;

    ContractDetails::ContractDetails(ContractDetails&& other) noexcept
        : clubName_(std::move(other.clubName_)),
          salary_(other.salary_),
          contractUntil_(std::move(other.contractUntil_)),
          isLoaned_(other.isLoaned_),
          loanEndDate_(std::move(other.loanEndDate_))
    {
        other.salary_ = 0.0;
        other.isLoaned_ = false;
        other.loanEndDate_.clear();
    }

    ContractDetails& ContractDetails::operator=(const ContractDetails& other)
    = default;

    ContractDetails& ContractDetails::operator=(
        ContractDetails&& other) noexcept
    {
        if (this != &other)
        {
            clubName_ = std::move(other.clubName_);
            salary_ = other.salary_;
            contractUntil_ = std::move(other.contractUntil_);
            isLoaned_ = other.isLoaned_;
            loanEndDate_ = std::move(other.loanEndDate_);

            other.salary_ = 0.0;
            other.isLoaned_ = false;
            other.loanEndDate_.clear();
        }
        return *this;
    }

    std::string ContractDetails::GetClubName() const { return clubName_; }
    double ContractDetails::GetSalary() const { return salary_; }

    std::string ContractDetails::GetContractUntil() const
    {
        return contractUntil_;
    }

    bool ContractDetails::IsPlayerOnLoan() const { return isLoaned_; }

    bool ContractDetails::IsContractValid() const
    {
        return ValidateIsoDate(contractUntil_);
    }

    bool ContractDetails::IsExpiringSoon() const
    {
        if (!IsContractValid()) return false;

        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        int cy = now->tm_year + 1900;
        int cm = now->tm_mon + 1;

        const int y = std::stoi(contractUntil_.substr(0, 4));
        const int m = std::stoi(contractUntil_.substr(5, 2));

        int diffMonths = (y - cy) * 12 + (m - cm);
        return diffMonths >= 0 && diffMonths <= 6;
    }

    std::string ContractDetails::GetLoanEndDate() const { return loanEndDate_; }

    void ContractDetails::SetClubName(const std::string& clubName)
    {
        if (clubName.empty())
            throw std::invalid_argument("Назва клубу не може бути порожньою.");
        clubName_ = clubName;
    }

    void ContractDetails::SetSalary(double salary)
    {
        if (salary < 0.0)
            throw std::invalid_argument("Зарплата не може бути від’ємною.");
        salary_ = salary;
    }

    void ContractDetails::SetContractUntil(const std::string& contractUntil)
    {
        if (!ValidateIsoDate(contractUntil))
            throw std::invalid_argument("Формат дати повинен бути YYYY-MM-DD.");
        contractUntil_ = contractUntil;
    }

    void ContractDetails::SetOnLoan(const std::string& loanEndDate)
    {
        if (!ValidateIsoDate(loanEndDate))
            throw std::invalid_argument(
                "Дата завершення оренди має формат YYYY-MM-DD.");

        isLoaned_ = true;
        loanEndDate_ = loanEndDate;

        std::cout << "[ІНФО] Гравця орендовано до " << loanEndDate_ << ".\n";
    }

    void ContractDetails::ReturnFromLoan()
    {
        isLoaned_ = false;
        loanEndDate_.clear();
        std::cout << "[ІНФО] Гравець повернувся з оренди.\n";
    }

    void ContractDetails::AdjustSalary(double percentage)
    {
        if (salary_ <= 0.0)
            throw std::logic_error(
                "Неможливо змінити зарплату: поточне значення ≤ 0.");

        const double factor = 1.0 + (percentage / 100.0);
        salary_ *= factor;

        std::cout << "[ІНФО] Зарплата змінена на " << percentage
            << "%. Нова зарплата: " << std::fixed << std::setprecision(2)
            << salary_ << " €\n";
    }

    void ContractDetails::ExtendContractDate(const std::string& newDate)
    {
        if (!ValidateIsoDate(newDate))
            throw std::invalid_argument("Формат дати має бути YYYY-MM-DD.");
        contractUntil_ = newDate;

        std::cout << "[ІНФО] Контракт продовжено до " << newDate << ".\n";
    }

    void ContractDetails::ShowDetails() const
    {
        std::cout << "\n=== Інформація про контракт ===\n";
        std::cout << "Клуб: " << clubName_
            << " | Зарплата: " << std::fixed << std::setprecision(2)
            << salary_ << " €\n";
        std::cout << "Контракт дійсний до: " << contractUntil_;
        if (isLoaned_) std::cout << " (Оренда до: " << loanEndDate_ << ")";
        std::cout << "\n";
    }
}
