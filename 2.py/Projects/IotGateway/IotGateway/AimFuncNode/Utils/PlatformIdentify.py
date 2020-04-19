import platform
platform_system=platform.system().lower()
def isWindows():
    return platform_system.find("win")>=0
def isLinux():
    return platform_system.find("lin")>=0