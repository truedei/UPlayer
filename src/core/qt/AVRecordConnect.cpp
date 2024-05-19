#include "AVRecordConnect.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

AVRecordConnect::AVRecordConnect()
{
    cs = GetAllVideoDevices();
}

QString AVRecordConnect::getAllVideoDeviceList() const
{
    json data2 = cs;
    return data2.dump(4).c_str();
}
