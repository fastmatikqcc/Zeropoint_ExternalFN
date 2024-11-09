namespace librarys
{
	HMODULE user32;
	HMODULE win32u;
	bool init()
	{
		HMODULE user32_lib = LoadLibraryA("user32.dll");
		if (!user32_lib) return false;
		HMODULE win32u_lib = LoadLibraryA("win32u.dll");
		if (!win32u_lib) return false;
		user32 = GetModuleHandleA("user32.dll");
		if (!user32) return false;
		win32u = GetModuleHandleA("win32u.dll");
		if (!win32u) return false;
		return true;
	}
}

namespace inputv2
{
	static BYTE ntusersendinput_bytes[30];
	static BOOLEAN WINAPI init()
	{
		LPVOID ntusersendinput_addr = GetProcAddress(librarys::user32, "NtUserSendInput");
		if (!ntusersendinput_addr)
		{
			ntusersendinput_addr = GetProcAddress(librarys::win32u, "NtUserSendInput");
			if (!ntusersendinput_addr) return FALSE;
		}
		memcpy(ntusersendinput_bytes, ntusersendinput_addr, 30);
		return TRUE;
	}
	static BOOLEAN WINAPI ntusersendinput(UINT cinputs, LPINPUT pinputs, int cbsize)
	{
		LPVOID ntusersendinput_spoof = VirtualAlloc(0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!ntusersendinput_spoof) return FALSE;
		memcpy(ntusersendinput_spoof, ntusersendinput_bytes, 30);
		NTSTATUS result = reinterpret_cast<NTSTATUS(NTAPI*)(UINT, LPINPUT, int)>(ntusersendinput_spoof)(cinputs, pinputs, cbsize);
		ZeroMemory(ntusersendinput_spoof, 0x1000);
		VirtualFree(ntusersendinput_spoof, 0, MEM_RELEASE);
		return (result > 0);
	}
	static BOOLEAN WINAPI move_mouse(int aX, int aY)
	{
		// Obfuscate variable names
		INPUT i1 = { 0 };
		i1.type = INPUT_MOUSE;
		i1.mi.mouseData = 0;
		i1.mi.time = 0;
		i1.mi.dx = aX;
		i1.mi.dy = aY;
		i1.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;

		// Obfuscate function name
		auto sendInputFunction = ntusersendinput;

		// Obfuscate arguments and call
		return sendInputFunction(1, &i1, sizeof(i1));
	}
}