#include "render.hpp"
#include <thread>
#include <mutex>
uintptr_t virtualaddy;
bool isProcessRunning(const std::string& processName) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (hSnap == INVALID_HANDLE_VALUE) return false;

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnap, &pe)) {
		do {
			if (processName == pe.szExeFile) {
				CloseHandle(hSnap);
				return true;
			}
		} while (Process32Next(hSnap, &pe));
	}

	CloseHandle(hSnap);
	return false;
}

void setColor(int textColor) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, textColor);
}

void TransparencyConsole()
{
	HWND console = GetConsoleWindow();
	DWORD style = GetWindowLong(console, GWL_EXSTYLE);
	style |= WS_EX_LAYERED;
	SetWindowLong(console, GWL_EXSTYLE, style);
	SetLayeredWindowAttributes(console, 0, 200, LWA_ALPHA);
}

class startup1 {
public:
    void Startup() {

        if (!kraft::GET_DRIVER()) {
            setColor(FOREGROUND_RED);
            std::cout << "[ERROR] ERROR CODE: 3 (Run Setup)" << std::endl;
            Sleep(2000);
            return;
        }
        else {
            setColor(FOREGROUND_GREEN);
            std::cout << "[SUCCESS] Driver Setup Successful\n";
            Sleep(2500);
        }
        system("cls");

        setColor(FOREGROUND_BLUE);
        std::cout << "[INFO] Launch Fortnite" << std::flush;

    
        while (!isProcessRunning("FortniteClient-Win64-Shipping.exe")) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        setColor(FOREGROUND_GREEN);
        std::cout << "\r" << std::string(30, ' ') << "\r";
        std::cout << "[SUCCESS] FORTNITE FOUND" << std::endl;

        kraft::GET_PROCESS("FortniteClient-Win64-Shipping.exe");

        if (!librarys::init())
        {
            printf("The librarys was not initialized");
            Sleep(3000);
            exit(0);
        }
        if (!inputv2::init())
        {
            printf("The input was not initialized");
            Sleep(3000);
            exit(0);
        }
        
        virtualaddy = kraft::GET_BASE_ADDRESS();

        //SonyDriverHelper::api::Init();

        kraft::GET_CR3();

        screen_width = GetSystemMetrics(SM_CXSCREEN);
        screen_height = GetSystemMetrics(SM_CYSCREEN);
     
        std::thread([&]() { for (;;) { g_cache->actors(); } }).detach();
        std::cout << "7" << std::endl;

        g_render->Overlay();
        std::cout << "8" << std::endl;
    }
};

static startup1* startup = new startup1;