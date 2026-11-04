#ifndef WINDOWSVERSIONEXT_H
#define WINDOWSVERSIONEXT_H

#include <QString>

namespace WindowsVersionExt {

enum class OS {
    Unknown,
    Windows10,
    Windows11,
    Server2019,
    Server2022
};

OS currentOS();
QString currentOSName();
QString currentOSVersion();

} //namespace WindowsVersionExt
#endif // WINDOWSVERSIONEXT_H
