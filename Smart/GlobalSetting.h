#ifndef GLOBALSETTING_H
#define GLOBALSETTING_H

#include <stdint.h>

#define SETTING_FILE "./System.ini"

class QSettings;
class GlobalSetting
{
public:
    static GlobalSetting *GetInstance();

    uint32_t master_id,devid;

private:
    QSettings *setting;

    GlobalSetting();
    ~GlobalSetting();
};

#endif // GLOBALSETTING_H
