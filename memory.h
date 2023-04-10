//#pragma once
//#include <fstream>
//#include <vector>
//
//typedef struct _shared_struct
//{
//	bool bRead;
//	bool bWrite;
//	bool bBaseAddress;
//
//	int pid;
//	uintptr_t address;
//	uintptr_t buffer;
//	uintptr_t size;
//
//} __shared_struct, * pshared_struct;
//
//class memory
//{
//	static inline HANDLE driver_handle;
//
//public:
//	static inline uintptr_t process_id;
//	static inline uintptr_t base_address;
//	static inline uintptr_t allocated_entry;
//
//	static bool initialize(int pid)
//	{
//		process_id = pid;
//
//		driver_handle = CreateFileA("\\\\.\\\Python", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
//
//		if (driver_handle == INVALID_HANDLE_VALUE)
//		{
//			utils::log("failed to initialize memory");
//			return false;
//		}
//
//		base_address = get_base_address();
//
//		return true;
//	}
//
//	static DWORD get_process_id(const wchar_t* process_name)
//	{
//		PROCESSENTRY32 pt = { sizeof(PROCESSENTRY32) };
//
//		HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//		Process32First(hsnap, &pt);
//
//		do {
//			if (!lstrcmpW(pt.szExeFile, process_name)) {
//				CloseHandle(hsnap);
//				return pt.th32ProcessID;
//			}
//		} while (Process32Next(hsnap, &pt));
//
//		CloseHandle(hsnap);
//		return 0;
//	}
//
//	static void read_physical_memory(void* address, void* buffer, size_t size)
//	{
//		_shared_struct req =
//		{
//			true,
//			false,
//			false,
//			process_id,
//			uintptr_t(address),
//			uintptr_t(buffer),
//			uintptr_t(size)
//		};
//
//		DeviceIoControl(driver_handle, 0, &req, sizeof(req), &req, sizeof(req), NULL, NULL);
//	}
//
//	template<typename read_t>
//	static read_t read(uintptr_t address, size_t size = sizeof(read_t))
//	{
//		read_t buffer{ };
//		read_physical_memory((void*)address, &buffer, size);
//		return buffer;
//	}
//
//	template<typename write_t>
//	static void write(uintptr_t address, write_t buffer)
//	{
//		_shared_struct req =
//		{
//			false,
//			true,
//			false,
//			process_id,
//			uintptr_t(address),
//			uintptr_t(&buffer),
//			sizeof(write_t)
//		};
//
//		DeviceIoControl(driver_handle, 0, &req, sizeof(req), &req, sizeof(req), NULL, NULL);
//	}
//
//	static uintptr_t get_base_address()
//	{
//		_shared_struct req =
//		{
//			false,
//			false,
//			true,
//			process_id,
//			0,
//			0,
//			0
//		};
//
//		DeviceIoControl(driver_handle, 0, &req, sizeof(req), &req, sizeof(req), NULL, NULL);
//
//		return (uintptr_t)req.address;
//	}
//};