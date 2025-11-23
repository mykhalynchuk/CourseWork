#include "../include/SeedDemo.h"
#include "../include/ClubManager.h"
#include "../include/FreeAgent.h"
#include "../include/ContractedPlayer.h"
#include "../include/Goalkeeper.h"
#include "../include/Utils.h"   // для enum Position

#include <memory>

namespace FootballManagement {

    void SeedDemoData(ClubManager& cm)
    {
        // приклади (можеш лишити як є або змінити):
        // Воротар
        auto bushchan = std::make_shared<Goalkeeper>(
            "Георгій Бущан", 30, "Україна", "Київ",
            1.96, 85.0, 7'000'000.0);
        cm.AddPlayer(bushchan);

        // Контрактний захисник
        auto zabarnyi = std::make_shared<ContractedPlayer>(
            "Ілля Забарний", 22, "Україна", "Київ",
            1.89, 80.0, 28'000'000.0, Position::Defender,
            1'500'000.0, "2029-06-30");
        zabarnyi->GetContractDetails().SetClubName("Bournemouth");
        cm.AddPlayer(zabarnyi);

        // Вільний агент — півзахисник
        auto malynovskyiFA = std::make_shared<FreeAgent>(
            "Руслан Маліновський", 31, "Україна", "Житомир",
            1.81, 79.0, 10'000'000.0, Position::Midfielder,
            1'200'000.0, "Genoa");
        cm.AddPlayer(malynovskyiFA);

        // Нападник контрактний
        auto yaremchuk = std::make_shared<ContractedPlayer>(
            "Роман Яремчук", 29, "Україна", "Львів",
            1.91, 82.0, 6'000'000.0, Position::Forward,
            1'000'000.0, "2027-06-30");
        yaremchuk->GetContractDetails().SetClubName("Valencia");
        cm.AddPlayer(yaremchuk);

        // Можеш додати ще кількох для тесту ...
    }

} // namespace FootballManagement
