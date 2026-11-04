#include "windowsversionext.h"

#include <windows.h>

namespace WindowsVersionExt {


struct VersionInfo {
    DWORD majorVersion = 0;
    DWORD minorVersion = 0;
    DWORD buildNumber = 0;
    WORD productType = 0;
    bool isValid = false;
};

static VersionInfo getRealVersionInfo()
{
    VersionInfo info;

    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll)
        return info;

    typedef LONG (WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

    auto pRtlGetVersion = reinterpret_cast<RtlGetVersionPtr>(
        GetProcAddress(hNtdll, "RtlGetVersion"));

    if (!pRtlGetVersion)
        return info;

    RTL_OSVERSIONINFOEXW osvi = { 0 };
    osvi.dwOSVersionInfoSize = sizeof(osvi);

    if (pRtlGetVersion(reinterpret_cast<PRTL_OSVERSIONINFOW>(&osvi)) != 0)
        return info;

    info.majorVersion = osvi.dwMajorVersion;
    info.minorVersion = osvi.dwMinorVersion;
    info.buildNumber = osvi.dwBuildNumber;
    info.productType = osvi.wProductType;
    info.isValid = true;

    return info;
}

OS currentOS()
{
    static const VersionInfo info = getRealVersionInfo();

    if (!info.isValid)
        return OS::Unknown;

    if (info.majorVersion != 10 || info.minorVersion != 0)
        return OS::Unknown;

    const DWORD build = info.buildNumber;

    const bool isServer = (info.productType != VER_NT_WORKSTATION);

    if (isServer) {
        // Windows Server 2022 начинается со сборки 20348
        if (build >= 20348)
            return OS::Server2022;

        // Windows Server 2019 имеет сборку 17763
        if (build == 17763)
            return OS::Server2019;

        return OS::Unknown;
    }

    // Клиентские версии: Windows 11 начинается со сборки 22000
    if (build >= 22000)
        return OS::Windows11;

    // Остальные сборки 10.0 считаем Windows 10
    return OS::Windows10;
}

QString currentOSName()
{
    switch (currentOS()) {
    case OS::Windows11:  return QStringLiteral("Windows 11");
    case OS::Server2022: return QStringLiteral("Windows Server 2022");
    case OS::Server2019: return QStringLiteral("Windows Server 2019");
    case OS::Windows10:  return QStringLiteral("Windows 10");
    default:             return QStringLiteral("Unknown");
    }
}

QString currentOSVersion()
{
    switch (currentOS()) {
    case OS::Windows11:  return QStringLiteral("11");
    case OS::Server2022: return QStringLiteral("server2022");
    case OS::Server2019: return QStringLiteral("server2019");
    case OS::Windows10:  return QStringLiteral("10");
    default:             return {};
    }
}

}//namespace WindowsVersionExt
