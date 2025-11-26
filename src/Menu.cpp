#include "../include/Menu.h"
#include "../include/SeedDemo.h"

#include "../include/ContractedPlayer.h"
#include "../include/FreeAgent.h"
#include "../include/Goalkeeper.h"
#include "../include/AuthManager.h"
#include "../include/FileManager.h"
#include "../include/Utils.h"
#include "../include/InputValidator.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <iomanip>
#include <limits>

namespace FootballManagement
{
// === Конструктор/деструктор ===
Menu::Menu(std::shared_ptr<ClubManager> clubManager)
   : clubManager_(std::move(clubManager)),
     fileManager_(),
     isRunning_(true)
{
}

// === Завантаження/збереження ===
void Menu::LoadAllData() const
{
   try {
      const auto userLines = fileManager_.LoadFromFile(USERS_FILE_NAME);
      AuthManager::GetInstance().DeserializeAllUsers(userLines);
      std::cout << "[ІНФО] Дані користувачів завантажено.\n";
   } catch (const std::exception& e) {
      std::cout << "[ПОМИЛКА] Не вдалося завантажити користувачів: "
                << e.what() << "\n";
   }

   try {
      const auto playerLines = fileManager_.LoadFromFile(PLAYERS_DATA_FILE);
      clubManager_->DeserializeAllPlayers(playerLines);
      std::cout << "[ІНФО] Дані гравців завантажено.\n";
   } catch (const std::exception& e) {
      std::cout << "[ПОМИЛКА] Не вдалося завантажити гравців: "
                << e.what() << "\n";
   }

   if (clubManager_->GetAll().empty()) {
      std::cout << "[ІНФО] Склад порожній — додаю демо-гравців.\n";
      SeedDemoData(*clubManager_);
      fileManager_.SaveToFile(PLAYERS_DATA_FILE, *clubManager_);
   }
}

void Menu::SaveAllData() const
{
   std::cout << "[ІНФО] Збереження даних...\n";
   fileManager_.SaveToFile(USERS_FILE_NAME, AuthManager::GetInstance());
   fileManager_.SaveToFile(PLAYERS_DATA_FILE, *clubManager_);
   std::cout << "[УСПІХ] Усі дані збережено.\n";
}

// === Головний цикл ===
void Menu::Run()
{
   LoadAllData();
   authenticateUser();

   if (!AuthManager::GetInstance().IsLoggedIn()) {
      std::cout << "\nСесію завершено. Програму закрито.\n";
      return;
   }

   while (isRunning_) {
      displayMainMenu();
      const int choice = InputValidator::GetIntInRange("Ваш вибір: ", 1, 9);
      handleMainMenu(choice);
   }
}

void Menu::Stop()
{
   isRunning_ = false;
   std::cout << "[ІНФО] Завершення роботи програми...\n";
}

// === Автентифікація ===
void Menu::authenticateUser()
{
   std::cout << "========================================\n";
   std::cout << "     СИСТЕМА КЕРУВАННЯ ФУТБОЛЬНИМ КЛУБОМ\n";
   std::cout << "========================================\n";

   while (true) {
      std::cout << "\nОберіть дію:\n";
      std::cout << "1) Увійти\n";
      std::cout << "2) Зареєструватися\n";
      std::cout << "3) Увійти як гість\n";
      std::cout << "4) Вийти з програми\n";

      int cmd = InputValidator::GetIntInRange("Ваш вибір: ", 1, 4);

      if (cmd == 1) {
         for (int attempts = 0; attempts < 3; ++attempts) {
            const std::string login = InputValidator::GetNonEmptyString("Логін: ");
            const std::string password = InputValidator::GetNonEmptyString("Пароль: ");
            if (AuthManager::GetInstance().Login(login, password))
               return;
            std::cout << "[ПОМИЛКА] Спробуйте ще раз.\n";
         }
         Stop(); return;
      } else if (cmd == 2) {
         const std::string login = InputValidator::GetNonEmptyString("Новий логін: ");
         const std::string password = InputValidator::GetNonEmptyString("Пароль (≥6 символів): ");
         const int roleInt = InputValidator::GetIntInRange("Роль (1=User, 0=Admin): ", 0, 1);
         const UserRole role = (roleInt == 0 ? UserRole::Admin : UserRole::StandardUser);

         if (AuthManager::GetInstance().Register(login, password, role)) {
            fileManager_.SaveToFile(USERS_FILE_NAME, AuthManager::GetInstance());
            std::cout << "[УСПІХ] Обліковий запис створено.\n";
         }
      } else if (cmd == 3) {
         AuthManager::GetInstance().LoginAsGuest();
         return;
      } else {
         Stop(); return;
      }
   }
}

// === Меню ===
void Menu::displayMainMenu() const
{
   std::cout << "\n=========== ГОЛОВНЕ МЕНЮ ===========\n";
   std::cout << "1. Керування гравцями (CRUD)\n";
   std::cout << "2. Переглянути всіх гравців\n";
   std::cout << "3. Пошук / Сортування / Фільтрація\n";
   std::cout << "4. Трансферний ринок (Підписати агента)\n";

   if (AuthManager::GetInstance().GetCurrentUser()->IsAdmin())
      std::cout << "5. Керування користувачами (Admin)\n";

   // --- НОВЕ ---
   std::cout << "6. Рейтинг і стати\n";

   std::cout << "7. Довідка\n";
   std::cout << "8. Зберегти всі дані\n";
   std::cout << "9. Вихід\n";
   std::cout << "====================================\n";
}

void Menu::handleMainMenu(int choice)
{
   const auto user = AuthManager::GetInstance().GetCurrentUser();
   const bool isAdmin = user->IsAdmin();

   switch (choice) {
      case 1: displayPlayerMenu(); break;
      case 2: viewPlayersFlow(); break;
      case 3: searchPlayerFlow(); break;
      case 4: transferFlow(); break;
      case 5:
         if (isAdmin) manageUsersFlow();
         else std::cout << "[ВІДМОВА] Необхідні права адміністратора.\n";
         break;

      case 6: {
         while (true) {
            std::cout << "\n--- РЕЙТИНГ / СТАТИ / ВАРТІСТЬ ---\n";
            std::cout << "1. Показати рейтинг + відсортувати\n";
            std::cout << "2. Редагувати стати гравця\n";
            std::cout << "3. Змінити ринкову вартість на % (1 / всі)\n";
            std::cout << "0. Назад\n";
            int c = InputValidator::GetIntInRange("Ваш вибір: ", 0, 3);
            if (c == 0) break;
            if (c == 1) RatingShowAndSortFlow();
            if (c == 2) EditPlayerStatsFlow();
            if (c == 3) AdjustMarketValuePercentFlow();
         }
         break;
      }

      case 7: displayHelp(); break;
      case 8: SaveAllData(); break;
      case 9: saveAndExit(); break;
      default: std::cout << "[ПОМИЛКА] Некоректний вибір.\n"; break;
   }
}

void Menu::saveAndExit()
{
   if (InputValidator::GetYesNoInput("Зберегти зміни перед виходом?"))
      SaveAllData();
   AuthManager::GetInstance().Logout();
   Stop();
}

void Menu::displayHelp() const
{
   std::cout << "\n--- ДОВІДКА ---\n";
   std::cout << "• Додавати, редагувати, видаляти гравців.\n";
   std::cout << "• Пошук, сортування, фільтрація.\n";
   std::cout << "• Підписання вільних агентів.\n";
   std::cout << "-----------------------------------\n";
   std::cout << "Натисніть Enter, щоб повернутись у меню...";
   std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
   std::cin.get();
}

// === Підменю гравців/CRUD ===
void Menu::displayPlayerMenu() const
{
   while (true) {
      std::cout << "\n==== МЕНЮ ГРАВЦІВ ====\n";
      std::cout << "1. Додати нового гравця\n";
      std::cout << "2. Видалити гравця\n";
      std::cout << "3. Редагувати гравця\n";
      std::cout << "0. Назад\n";

      int choice = InputValidator::GetIntInRange("Ваш вибір: ", 0, 3);
      if (choice == 0) return;
      if (choice == 1) const_cast<Menu*>(this)->addPlayerFlow();
      if (choice == 2) const_cast<Menu*>(this)->deletePlayerFlow();
      if (choice == 3) const_cast<Menu*>(this)->editPlayerFlow();
   }
}

void Menu::addPlayerFlow()
{
   std::cout << "\n--- ДОДАВАННЯ ГРАВЦЯ ---\n";
   std::string name = InputValidator::GetNonEmptyString("Ім'я (0 - назад): ");
   if (name == "0") return;

   int age = InputValidator::GetIntInRange("Вік: ", 16, 45);
   std::string nationality =
      InputValidator::GetNonEmptyString("Національність (0 - назад): ");
   if (nationality == "0") return;

   std::string origin =
      InputValidator::GetNonEmptyString("Походження (0 - назад): ");
   if (origin == "0") return;

   double height = InputValidator::GetDoubleInput("Зріст (см): ");
   double weight = InputValidator::GetDoubleInput("Вага (кг): ");
   double value  = InputValidator::GetDoubleInput("Ринкова вартість: ");

   int pos = InputValidator::GetIntInRange(
      "Позиція (0=GK, 1=DEF, 2=MID, 3=FWD): ", 0, 3);
   const Position position = static_cast<Position>(pos);

   if (position == Position::Goalkeeper) {
      auto gk = std::make_shared<Goalkeeper>(
         name, age, nationality, origin, height, weight, value);
      clubManager_->AddPlayer(gk);
      std::cout << "[УСПІХ] Воротаря додано.\n";
      return;
   }

   int type = InputValidator::GetIntInRange(
      "Тип (1=Контрактний, 2=Вільний агент, 0=Назад): ", 0, 2);
   if (type == 0) return;

   if (type == 1) {
      double salary = InputValidator::GetDoubleInput("Річна зарплата: ");
      std::string contractUntil =
         InputValidator::GetNonEmptyString("Контракт до (YYYY-MM-DD) (0 - назад): ");
      if (contractUntil == "0") return;

      auto p = std::make_shared<ContractedPlayer>(
         name, age, nationality, origin, height, weight, value,
         position, salary, contractUntil);
      clubManager_->AddPlayer(p);
      std::cout << "[УСПІХ] Контрактного гравця додано.\n";
   } else {
      double expectedSalary = InputValidator::GetDoubleInput("Очікувана зарплата: ");
      std::string lastClub =
         InputValidator::GetNonEmptyString("Останній клуб (0 - назад): ");
      if (lastClub == "0") return;

      auto a = std::make_shared<FreeAgent>(
         name, age, nationality, origin, height, weight, value,
         position, expectedSalary, lastClub);
      clubManager_->AddPlayer(a);
      std::cout << "[УСПІХ] Вільного агента додано.\n";
   }
}

void Menu::editPlayerFlow()
{
   std::cout << "\n--- РЕДАГУВАННЯ ГРАВЦЯ ---\n";
   int id = InputValidator::GetIntInput("Введіть ID гравця (0 - назад): ");
   if (id == 0) return;

   auto& players = clubManager_->GetAll();
   std::shared_ptr<Player> player = nullptr;

   for (const auto& p : players) {
      if (p && p->GetPlayerId() == id) { player = p; break; }
   }
   if (!player) {
      std::cout << "[ПОМИЛКА] Гравця з таким ID не знайдено.\n";
      return;
   }

   std::cout << "\nПоточні дані:\n";
   player->ShowInfo();

   std::cout << "\nЩо змінити?\n";
   std::cout << "1. Ім’я\n";
   std::cout << "2. Вік\n";
   std::cout << "3. Національність\n";
   std::cout << "4. Ринкова вартість\n";
   std::cout << "0. Скасувати\n";

   int field = InputValidator::GetIntInRange("Оберіть поле: ", 0, 4);
   if (field == 0) return;

   switch (field) {
      case 1: {
         std::string newName = InputValidator::GetNonEmptyString("Нове ім’я: ");
         player->SetName(newName); break;
      }
      case 2: {
         int newAge = InputValidator::GetIntInRange("Новий вік: ", 16, 45);
         player->SetAge(newAge); break;
      }
      case 3: {
         std::string newNat =
            InputValidator::GetNonEmptyString("Нова національність: ");
         player->SetNationality(newNat); break;
      }
      case 4: {
         double newVal = InputValidator::GetDoubleInput("Нова ринкова вартість: ");
         player->SetMarketValue(newVal); break;
      }
      default: return;
   }

   std::cout << "[УСПІХ] Дані оновлено!\n";
   SaveAllData();
}

void Menu::deletePlayerFlow() const
{
   std::string raw = InputValidator::GetNonEmptyString("ID гравця (0 - назад): ");
   if (raw == "0") return;

   try {
      int id = std::stoi(raw);
      clubManager_->RemovePlayers(id);
      std::cout << "[УСПІХ] Якщо гравець існував — видалено.\n";
   } catch (...) {
      std::cout << "[ПОМИЛКА] Невірний формат ID.\n";
   }
}

void Menu::viewPlayersFlow() const
{
   clubManager_->ViewAllPlayers();
   std::cout << "Натисніть Enter, щоб повернутись у меню...";
   std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
   std::cin.get();
}

// === Пошук/Сортування/Фільтрація ===
void Menu::searchPlayerFlow() const
{
   while (true) {
      std::cout << "\n--- ПОШУК / СОРТУВАННЯ / ФІЛЬТРАЦІЯ ---\n";
      std::cout << "1. Пошук за ім’ям\n";
      std::cout << "2. Сортування за рейтингом\n";
      std::cout << "3. Фільтрація за статусом\n";
      std::cout << "0. Назад\n";

      int choice = InputValidator::GetIntInRange("Ваш вибір: ", 0, 3);
      if (choice == 0) return;

      if (choice == 1) {
         std::string q =
            InputValidator::GetNonEmptyString("Ім’я або частина (0 - назад): ");
         if (q == "0") continue;
         const auto res = clubManager_->SearchByName(q);
         for (const auto& p : res) { p->ShowInfo(); std::cout << "------------------------\n"; }
         std::cout << "Натисніть Enter, щоб продовжити...";
         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
         std::cin.get();
      } else if (choice == 2) {
         clubManager_->SortByPerformanceRating();
         clubManager_->ViewAllPlayers();
         std::cout << "Натисніть Enter, щоб продовжити...";
         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
         std::cin.get();
      } else if (choice == 3) {
         std::string st = InputValidator::GetNonEmptyString("Статус (0 - назад): ");
         if (st == "0") continue;
         const auto res = clubManager_->FilterByStatus(st);
         for (const auto& p : res) { p->ShowInfo(); std::cout << "------------------------\n"; }
         std::cout << "Натисніть Enter, щоб продовжити...";
         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
         std::cin.get();
      }
   }
}

// === Трансферний ринок ===
void Menu::transferFlow() const
{
   std::cout << "\n--- ТРАНСФЕРНИЙ РИНОК ---\n";
   std::string nameQuery =
      InputValidator::GetNonEmptyString("Пошук агента за ім’ям (0 - назад): ");
   if (nameQuery == "0") return;

   const auto found = clubManager_->SearchByName(nameQuery);
   std::shared_ptr<FreeAgent> agent = nullptr;
   for (const auto& p : found) {
      agent = std::dynamic_pointer_cast<FreeAgent>(p);
      if (agent && agent->IsAvailableForNegotiation()) break;
   }

   if (!agent) {
      std::cout << "[ПОМИЛКА] Доступного вільного агента не знайдено.\n";
      return;
   }

   std::cout << "[ІНФО] Знайдено агента: " << agent->GetName() << "\n";
   double offer = InputValidator::GetDoubleInput("Запропонована зарплата: ");
   std::string until =
      InputValidator::GetNonEmptyString("Контракт до (YYYY-MM-DD) (0 - назад): ");
   if (until == "0") return;

   if (clubManager_->SignFreeAgent(agent, offer, until))
      std::cout << "[УСПІХ] Контракт підписано.\n";
   else
      std::cout << "[ПОМИЛКА] Не вдалося підписати контракт.\n";
}

// === Користувачі (адмін) ===
void Menu::manageUsersFlow()
{
   while (true) {
      std::cout << "\n--- КЕРУВАННЯ КОРИСТУВАЧАМИ ---\n";
      AuthManager::GetInstance().ViewAllUsers();

      std::cout << "1. Створити користувача\n";
      std::cout << "2. Видалити користувача\n";
      std::cout << "0. Назад\n";

      int choice = InputValidator::GetIntInRange("Ваш вибір: ", 0, 2);
      if (choice == 0) return;

      if (choice == 1) {
         adminCreateUser();
      } else {
         std::string login =
            InputValidator::GetNonEmptyString("Логін для видалення (0 - назад): ");
         if (login == "0") continue;

         if (InputValidator::GetYesNoInput("Підтвердити видалення?")) {
            AuthManager::GetInstance().DeleteUser(login);
            fileManager_.SaveToFile(USERS_FILE_NAME, AuthManager::GetInstance());
            std::cout << "[УСПІХ] Видалено (якщо існував).\n";
         } else {
            std::cout << "[СКАСОВАНО]\n";
         }
      }
   }
}

void Menu::adminCreateUser()
{
   std::string login =
      InputValidator::GetNonEmptyString("Новий логін (0 - назад): ");
   if (login == "0") return;

   std::string password =
      InputValidator::GetNonEmptyString("Пароль (0 - назад): ");
   if (password == "0") return;

   int role = InputValidator::GetIntInRange("Роль (1=Admin, 2=User, 0=Назад): ", 0, 2);
   if (role == 0) return;

   const bool ok = AuthManager::GetInstance().Register(
      login, password, (role == 1 ? UserRole::Admin : UserRole::StandardUser));

   if (ok) {
      fileManager_.SaveToFile(USERS_FILE_NAME, AuthManager::GetInstance());
      std::cout << "[УСПІХ] Користувача створено.\n";
   } else {
      std::cout << "[ПОМИЛКА] Не вдалося створити (логін може існувати).\n";
   }
}

// === НОВЕ: рейтинг/стати/вартість ===
void Menu::RatingShowAndSortFlow()
{
   auto& players = clubManager_->GetAll();
   if (players.empty()) {
      std::cout << "[ПОПЕРЕДЖЕННЯ] Немає гравців у складі.\n";
      return;
   }

   std::cout << "\n--- ПОТОЧНИЙ РЕЙТИНГ (перед сортуванням) ---\n";
   for (const auto& p : players) {
      if (!p) continue;
      std::cout << "ID " << p->GetPlayerId() << " | "
                << p->GetName() << " | рейтинг = "
                << std::fixed << std::setprecision(2)
                << p->CalculatePerformanceRating() << "\n";
   }

   clubManager_->SortByPerformanceRating(); // спадно
   std::cout << "\n[ІНФО] Відсортовано за рейтингом (спадно).\n";
   clubManager_->ViewAllPlayers();
}

void Menu::AdjustMarketValuePercentFlow()
{
   std::cout << "\n--- РИНКОВА ВАРТІСТЬ: ЗМІНА НА % ---\n";
   std::cout << "1. Змінити одному гравцю\n";
   std::cout << "2. Змінити всім гравцям\n";
   std::cout << "0. Назад\n";
   int m = InputValidator::GetIntInRange("Ваш вибір: ", 0, 2);
   if (m == 0) return;

   double percent =
      InputValidator::GetDoubleInput("Відсоток (наприклад, 10 або -5): ");

   auto& players = clubManager_->GetAll();
   if (players.empty()) {
      std::cout << "[ПОПЕРЕДЖЕННЯ] Немає гравців.\n";
      return;
   }

   if (m == 1) {
      int id = InputValidator::GetIntInput("ID гравця: ");
      for (const auto& p : players) {
         if (p && p->GetPlayerId() == id) {
            double cur = p->GetMarketValue();
            double nv  = cur * (1.0 + percent / 100.0);
            p->SetMarketValue(nv);
            std::cout << "[УСПІХ] Оновлено MV гравця " << p->GetName()
                      << " з " << cur << " до " << nv << ".\n";
            SaveAllData();
            return;
         }
      }
      std::cout << "[ПОМИЛКА] Гравця з таким ID не знайдено.\n";
      return;
   }

   // Маса: всім
   for (const auto& p : players) {
      if (!p) continue;
      const double cur = p->GetMarketValue();
      p->SetMarketValue(cur * (1.0 + percent / 100.0));
   }
   std::cout << "[УСПІХ] Оновлено MV усім гравцям на " << percent << "%.\n";
   SaveAllData();
}

void Menu::EditPlayerStatsFlow()
{
   std::cout << "\n--- РЕДАГУВАННЯ СТАТІВ ---\n";
   int id = InputValidator::GetIntInput("ID гравця (0 - назад): ");
   if (id == 0) return;

   auto& players = clubManager_->GetAll();
   std::shared_ptr<Player> base = nullptr;
   for (const auto& p : players) {
      if (p && p->GetPlayerId() == id) { base = p; break; }
   }
   if (!base) {
      std::cout << "[ПОМИЛКА] Гравця не знайдено.\n";
      return;
   }

   if (auto gk = std::dynamic_pointer_cast<Goalkeeper>(base)) {
      std::cout << "Виявлено позицію: Воротар (GK).\n";
      int saves        = InputValidator::GetIntInRange("Сейви: ", 0, 2000);
      int cleanSheets  = InputValidator::GetIntInRange("«Сухі» матчі: ", 0, 2000);
      int conceded     = InputValidator::GetIntInRange("Пропущені голи: ", 0, 2000);

      // TODO: розкоментуйте та підставте точні назви ваших сеттерів у Goalkeeper:
      // gk->SetSaves(saves);
      // gk->SetCleanSheets(cleanSheets);
      // gk->SetGoalsConceded(conceded);

      std::cout << "[ІНФО] Стати GK введені. Під’єднайте сеттери у Goalkeeper.\n";
      SaveAllData();
      return;
   }

   std::cout << "Виявлено позицію: Польовий гравець.\n";
   int goals   = InputValidator::GetIntInRange("Голи: ", 0, 2000);
   int assists = InputValidator::GetIntInRange("Асисти: ", 0, 2000);
   int matches = InputValidator::GetIntInRange("Матчі: ", 0, 2000);

   if (auto cp = std::dynamic_pointer_cast<ContractedPlayer>(base)) {
      // TODO: підстав свої методи у ContractedPlayer:
      // cp->SetGoals(goals);
      // cp->SetAssists(assists);
      // cp->SetMatches(matches);
   } else if (auto fa = std::dynamic_pointer_cast<FreeAgent>(base)) {
      // TODO: підстав свої методи у FreeAgent:
      // fa->SetGoals(goals);
      // fa->SetAssists(assists);
      // fa->SetMatches(matches);
   } else {
      std::cout << "[ПОПЕРЕДЖЕННЯ] Тип гравця не підтримано для редагування статів.\n";
      return;
   }

   std::cout << "[ІНФО] Стати польового введені. Під’єднайте сеттери у похідних.\n";
   SaveAllData();
}

} // namespace FootballManagement
