#include "EndingJudge.h"
#include "core/Player.h"

namespace SA {

EndingType EndingJudge::judge(const Player* player, bool choseToStay) {
    // 优先级 1：压力崩溃 → End5
    if (player->stress() >= STRESS_THRESHOLD_BREAK) {
        return EndingType::End5_Hospital;
    }

    // 优先级 2：线代黑化 → End4
    if (player->darkness() >= DARKNESS_THRESHOLD_LA) {
        return EndingType::End4_EternalLA;
    }

    // 优先级 3：好感度达标 → End2 / End3
    int maxAffinity = 0;
    for (auto s : {SubjectType::ProgDesign, SubjectType::Calculus,
                   SubjectType::LinearAlgebra, SubjectType::AIIntro}) {
        if (player->affinity(s) > maxAffinity) {
            maxAffinity = player->affinity(s);
        }
    }
    if (maxAffinity >= AFFINITY_THRESHOLD_LOVE) {
        return choseToStay ? EndingType::End2_BestLove
                            : EndingType::End3_Farewell;
    }

    // 默认：End1
    return EndingType::End1_GoodGrade;
}

} // namespace SA
