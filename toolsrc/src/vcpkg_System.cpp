#include "pch.h"
#include "vcpkg_System.h"

namespace vcpkg::System
{
    fs::path get_exe_path_of_current_process()
    {
        wchar_t buf[_MAX_PATH ];
        int bytes = GetModuleFileNameW(nullptr, buf, _MAX_PATH);
        if (bytes == 0)
            std::abort();
        return fs::path(buf, buf + bytes);
    }

    int cmd_execute(const wchar_t* cmd_line)
    {
        // Flush stdout before launching external process
        _flushall();

        const wchar_t* env[] = {
            LR"(ALLUSERSPROFILE=C:\ProgramData)",
            LR"(APPDATA=C:\Users\roschuma\AppData\Roaming)",
            LR"(ChocolateyPath=C:\Chocolatey)",
            LR"(CommonProgramFiles=C:\Program Files\Common Files)",
            LR"(CommonProgramFiles(x86)=C:\Program Files (x86)\Common Files)",
            LR"(CommonProgramW6432=C:\Program Files\Common Files)",
            LR"(COMPUTERNAME=ROSCHUMA-005D)",
            LR"(ComSpec=C:\WINDOWS\system32\cmd.exe)",
            LR"(HOMEDRIVE=C:)",
            LR"(HOMEPATH=\Users\roschuma)",
            LR"(LOCALAPPDATA=C:\Users\roschuma\AppData\Local)",
            LR"(LOGONSERVER=\\ROSCHUMA-005D)",
            LR"(MSMPI_BIN=C:\Program Files\Microsoft MPI\Bin\)",
            LR"(NUMBER_OF_PROCESSORS=8)",
            LR"(OneDrive=C:\Users\roschuma\OneDrive)",
            LR"(OS=Windows_NT)",
            LR"(Path=C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\Users\roschuma\AppData\Local\Microsoft\WindowsApps;C:\Program Files\CMake\bin)",
            LR"(PATHEXT=.COM;.EXE;.BAT;.CMD;.VBS;.VBE;.JS;.JSE;.WSF;.WSH;.MSC;.CPL)",
            LR"(PROCESSOR_ARCHITECTURE=AMD64)",
            LR"(PROCESSOR_IDENTIFIER=Intel64 Family 6 Model 60 Stepping 3, GenuineIntel)",
            LR"(PROCESSOR_LEVEL=6)",
            LR"(PROCESSOR_REVISION=3c03)",
            LR"(ProgramData=C:\ProgramData)",
            LR"(ProgramFiles=C:\Program Files)",
            LR"(ProgramFiles(x86)=C:\Program Files (x86))",
            LR"(ProgramW6432=C:\Program Files)",
            LR"(PROMPT=$P$G)",
            LR"(PSModulePath=C:\Users\roschuma\Documents\WindowsPowerShell\Modules;C:\Program Files\WindowsPowerShell\Modules;C:\WINDOWS\system32\WindowsPowerShell\v1.0\Modules)",
            LR"(PUBLIC=C:\Users\Public)",
            LR"(SystemDrive=C:)",
            LR"(SystemRoot=C:\WINDOWS)",
            LR"(TEMP=C:\Users\roschuma\AppData\Local\Temp)",
            LR"(TMP=C:\Users\roschuma\AppData\Local\Temp)",
            LR"(USERDNSDOMAIN=redmond.corp.microsoft.com)",
            LR"(USERDOMAIN=REDMOND)",
            LR"(USERDOMAIN_ROAMINGPROFILE=REDMOND)",
            LR"(USERNAME=roschuma)",
            LR"(USERPROFILE=C:\Users\roschuma)",
            LR"(VS140COMNTOOLS=C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\)",
            LR"(windir=C:\WINDOWS)",
            nullptr,
        };


        // Basically we are wrapping it in quotes
        const std::wstring& actual_cmd_line = Strings::wformat(LR"###("%s")###", cmd_line);
        //int exit_code = _wsystem(actual_cmd_line.c_str());
        auto exit_code = _wspawnlpe(_P_WAIT, L"cmd.exe", L"cmd.exe", L"/c", actual_cmd_line.c_str(), nullptr, env);
        return exit_code;
    }

    exit_code_and_output cmd_execute_and_capture_output(const wchar_t* cmd_line)
    {
        // Flush stdout before launching external process
        fflush(stdout);

        const std::wstring& actual_cmd_line = Strings::wformat(LR"###("%s")###", cmd_line);

        std::string output;
        char buf[1024];
        auto pipe = _wpopen(actual_cmd_line.c_str(), L"r");
        if (pipe == nullptr)
        {
            return { 1, output };
        }
        while (fgets(buf, 1024, pipe))
        {
            output.append(buf);
        }
        if (!feof(pipe))
        {
            return { 1, output };
        }
        auto ec = _pclose(pipe);
        return { ec, output };
    }

    std::wstring create_powershell_script_cmd(const fs::path& script_path)
    {
        return create_powershell_script_cmd(script_path, L"");
    }

    std::wstring create_powershell_script_cmd(const fs::path& script_path, const std::wstring& args)
    {
        // TODO: switch out ExecutionPolicy Bypass with "Remove Mark Of The Web" code and restore RemoteSigned
       return Strings::wformat(LR"(powershell -ExecutionPolicy Bypass -Command "& {& '%s' %s}")", script_path.native(), args);
    }

    void print(const char* message)
    {
        fputs(message, stdout);
    }

    void println(const char* message)
    {
        print(message);
        putchar('\n');
    }

    void print(const color c, const char* message)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo{};
        GetConsoleScreenBufferInfo(hConsole, &consoleScreenBufferInfo);
        auto original_color = consoleScreenBufferInfo.wAttributes;

        SetConsoleTextAttribute(hConsole, static_cast<WORD>(c) | (original_color & 0xF0));
        print(message);
        SetConsoleTextAttribute(hConsole, original_color);
    }

    void println(const color c, const char* message)
    {
        print(c, message);
        putchar('\n');
    }

    optional<std::wstring> get_environmental_variable(const wchar_t* varname) noexcept
    {
        wchar_t* buffer;
        _wdupenv_s(&buffer, nullptr, varname);

        if (buffer == nullptr)
        {
            return nullptr;
        }
        std::unique_ptr<wchar_t, void(__cdecl *)(void*)> bufptr(buffer, free);
        return std::make_unique<std::wstring>(buffer);
    }

    void set_environmental_variable(const wchar_t* varname, const wchar_t* varvalue) noexcept
    {
        _wputenv_s(varname, varvalue);
    }
}
