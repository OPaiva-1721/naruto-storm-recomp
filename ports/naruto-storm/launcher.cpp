/*
 * NarutoStorm.exe front end.
 *
 * The toolkit's boot harness (boot_main.cpp, built here with its main renamed
 * to ps3_harness_main) takes the decrypted EBOOT on the command line and reads
 * everything else from environment variables. A player should not have to set
 * any of that, so this reads NarutoStorm.ini next to the executable, fills in
 * the variables the title needs (only those the user has not set already, so
 * a debugging session still overrides them), and hands the harness the EBOOT
 * path. A missing ini is written out with the defaults.
 *
 *   game  = folder of the extracted disc (the one holding PS3_GAME)
 *   hdd0  = folder used as the PS3's internal drive: the installed game data
 *           and the saves (home/00000001/savedata) live here
 *   log   = 0 quiet (default), 1 full diagnostic log on stderr
 *
 * Relative paths are relative to the executable's folder.
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

int ps3_harness_main(int argc, char** argv);

static std::string exe_dir()
{
    char p[MAX_PATH];
    DWORD n = GetModuleFileNameA(NULL, p, MAX_PATH);
    std::string s(p, n);
    size_t k = s.find_last_of("\\/");
    return k == std::string::npos ? std::string(".") : s.substr(0, k);
}

/* Absolute and without "..": the runtime compares and splices these paths as
 * strings, and "<exe dir>\..\naruto-hdd0" made the install check miss the
 * installed data and run the whole installation again. */
static std::string resolve(const std::string& base, std::string v)
{
    if (v.empty()) return v;
    for (char& c : v) if (c == '/') c = '\\';
    if (!(v.size() > 1 && (v[1] == ':' || (v[0] == '\\' && v[1] == '\\'))))
        v = base + "\\" + v;
    char full[MAX_PATH];
    DWORD n = GetFullPathNameA(v.c_str(), MAX_PATH, full, NULL);
    return (n && n < MAX_PATH) ? std::string(full, n) : v;
}

static std::string trim(const std::string& s)
{
    size_t a = s.find_first_not_of(" \t\r\n\""), b = s.find_last_not_of(" \t\r\n\"");
    return a == std::string::npos ? std::string() : s.substr(a, b - a + 1);
}

static bool file_exists(const std::string& p)
{
    DWORD a = GetFileAttributesA(p.c_str());
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}

/* Set only when the user has not: a value in the environment always wins. */
static void set_default(const char* name, const std::string& value)
{
    if (!getenv(name)) _putenv_s(name, value.c_str());
}

static std::string g_elf;      /* set by the early configuration below */
static int g_config_rc = 0;

static void configure(void)
{
    const std::string dir = exe_dir();
    const std::string ini = dir + "\\NarutoStorm.ini";
    std::string game = "game", hdd0 = "dev_hdd0", log = "0";
    FILE* f = fopen(ini.c_str(), "r");
    if (f) {
        char line[1024];
        while (fgets(line, sizeof line, f)) {
            std::string l = trim(line);
            if (l.empty() || l[0] == '#' || l[0] == ';' || l[0] == '[') continue;
            size_t eq = l.find('=');
            if (eq == std::string::npos) continue;
            std::string k = trim(l.substr(0, eq)), v = trim(l.substr(eq + 1));
            if (k == "game") game = v;
            else if (k == "hdd0") hdd0 = v;
            else if (k == "log") log = v;
        }
        fclose(f);
    } else if ((f = fopen(ini.c_str(), "w")) != NULL) {
        fputs("# Naruto: Ultimate Ninja Storm -- PC port settings\n"
              "# Relative paths are relative to this folder.\n"
              "\n"
              "# Folder of your extracted disc (the one that holds PS3_GAME).\n"
              "# PS3_GAME\\USRDIR\\EBOOT.elf must be the decrypted executable.\n"
              "game = game\n"
              "\n"
              "# The PS3's internal drive: installed game data and your saves.\n"
              "hdd0 = dev_hdd0\n"
              "\n"
              "# 1 = write the full diagnostic log to the console (slow).\n"
              "log = 0\n", f);
        fclose(f);
    }

    const std::string game_dir = resolve(dir, game);
    const std::string hdd0_dir = resolve(dir, hdd0);
    std::string elf = game_dir + "\\PS3_GAME\\USRDIR\\EBOOT.elf";
    if (!file_exists(elf)) {
        const std::string alt = game_dir + "\\USRDIR\\EBOOT.elf";   /* "game" pointed at PS3_GAME */
        if (file_exists(alt)) elf = alt;
    }
    if (!file_exists(elf)) {
        char msg[1600];
        snprintf(msg, sizeof msg,
                 "Could not find the decrypted game executable:\n\n%s\n\n"
                 "Set \"game\" in NarutoStorm.ini to the folder of your extracted disc.",
                 elf.c_str());
        MessageBoxA(NULL, msg, "Naruto: Ultimate Ninja Storm", MB_ICONERROR | MB_OK);
        g_config_rc = 2;
        return;
    }
    CreateDirectoryA(hdd0_dir.c_str(), NULL);

    set_default("PS3_HDD0_ROOT", hdd0_dir);
    /* The title waits for cellSail to play its PAMF movies; until they are
     * decoded, report each one as started and finished. */
    set_default("PS3_SAIL_AUTOSTART", "1");
    /* The title fences its streamed vertex data with NV406E semaphores; when
     * ACQUIRE does not wait, the RSX reads last frame's buffers and the mode
     * select screen draws nothing. */
    set_default("GCM_SEMA_ACQUIRE", "1");
    /* The per-event log is taken through one FILE lock by every guest thread;
     * left on it starves the audio thread and the music breaks up. */
    set_default("PS3_VERBOSE", log == "1" ? "1" : "0");

    g_elf = elf;
}

int main(int argc, char** argv)
{
    /* Anything on the command line means a developer run (or the child below):
     * hand it straight to the harness. */
    if (argc > 1) return ps3_harness_main(argc, argv);

    configure();
    if (g_config_rc) return g_config_rc;

    /* Start the game as a child that inherits the environment set above.
     * Setting it here and calling the harness in-process is too late: runtime
     * code logs from static constructors, and ps3_log_verbose() latches
     * PS3_VERBOSE on its first call -- the log stayed on and the per-event
     * lines starved the audio thread. */
    char self[MAX_PATH];
    GetModuleFileNameA(NULL, self, MAX_PATH);
    std::string cmd = std::string("\"") + self + "\" \"" + g_elf + "\"";
    STARTUPINFOA si; PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof si); si.cb = sizeof si;
    ZeroMemory(&pi, sizeof pi);
    if (!CreateProcessA(self, &cmd[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        char* hargv[] = { argv[0], const_cast<char*>(g_elf.c_str()), NULL };
        return ps3_harness_main(2, hargv);
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD rc = 0;
    GetExitCodeProcess(pi.hProcess, &rc);
    CloseHandle(pi.hThread); CloseHandle(pi.hProcess);
    return (int)rc;
}
