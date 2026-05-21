#pragma once

#include <QString>

namespace SA {

class SaveManager
{
public:
    static bool saveGame(const QString& path);

    static bool loadGame(const QString& path);
};

} // namespace SA