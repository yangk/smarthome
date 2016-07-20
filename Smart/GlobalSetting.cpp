#include <QSettings>
#include "GlobalSetting.h"

GlobalSetting::GlobalSetting()
{
    setting = new QSettings(SETTING_FILE, QSettings::IniFormat);

    master_id = setting->value("Modem/MasterID", 1).toInt();
    devid  = setting->value("Modem/SlaveID", 2).toInt();
}
GlobalSetting::~GlobalSetting()
{
    setting->setValue("Modem/MasterID", master_id);
    setting->setValue("Modem/SlaveID", devid);
    delete setting;
}


GlobalSetting *GlobalSetting::GetInstance()
{
    static GlobalSetting _setting;

    return &_setting;
}
