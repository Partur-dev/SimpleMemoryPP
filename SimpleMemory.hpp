/*

	MIT License

	Copyright (c) 2020 1nfinit

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

*/

//TODO: External process (sm::external)
//TODO: Memory backup/restore

//IDEA: Linux support
//IDEA: Call function
//IDEA: 64bit memory
//IDEA: Memory stream

#pragma once

#ifndef __SIMPLEMEMORY_HPP__
#define __SIMPLEMEMORY_HPP__

#include <Windows.h>
#include <vector>

#define SM_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

typedef unsigned int sm_address_t;
typedef unsigned long sm_dword_t;
typedef unsigned long sm_size_t;
typedef unsigned char sm_byte_t;
typedef void *sm_handle_t;

template<typename T>
using sm_container_t = std::vector<T>;

enum SM_PROTECT : sm_dword_t
{
	SM_NO_ACCESS = PAGE_NOACCESS,

	SM_EXECUTE_ACCESS = PAGE_EXECUTE,
	SM_READ_ACCESS = PAGE_READONLY,
	SM_WRITE_ACCESS = PAGE_WRITECOPY,

	SM_READ_WRITE_ACCESS = PAGE_READWRITE,
	SM_EXECUTE_READ_ACCESS = PAGE_EXECUTE_READ,
	SM_EXECUTE_WRITE_ACCESS = PAGE_EXECUTE_WRITECOPY,

	SM_EXECUTE_READ_WRITE_ACCESS = PAGE_EXECUTE_READWRITE
};

enum SM_PROCESS
{
	SM_PROCESS_NONE,
	SM_PROCESS_ID,
	SM_PROCESS_FILENAME,
	SM_PROCESS_WINDOWNAME
};

template<typename ToT, typename FromT>
constexpr ToT sm_convert(FromT value) {
	union _SM_Convert_t_
	{
		FromT in; ToT out;
	};
	return _SM_Convert_t_{ value }.out;
}

namespace sm
{
	namespace internal
	{
		sm_dword_t process_id() {
			return sm_convert<sm_dword_t>(GetCurrentProcessId());
		}

		sm_handle_t process() {
			return sm_convert<sm_handle_t>(GetCurrentProcess());
		}

		sm_address_t module_address(const char *moduleName) {
			return sm_convert<sm_address_t>(GetModuleHandleA(moduleName));
		}

		sm_address_t base_address() {
			return sm::internal::module_address(0);
		}

		SM_PROTECT protect(sm_address_t address, sm_size_t size, SM_PROTECT protect) {
			SM_PROTECT _protect;
			VirtualProtect((LPVOID)address, (SIZE_T)size, (DWORD)protect, (PDWORD)&_protect);
			return _protect;
		}

		template<typename T, bool UseProtect = true, typename AddressT>
		void write(AddressT address, const T &value) {
			const auto _address = sm_convert<sm_address_t>(address);
			const auto _size = sizeof(T);
			SM_PROTECT _protect;

			if (UseProtect)
				_protect = sm::internal::protect(_address, _size, SM_WRITE_ACCESS);

			*reinterpret_cast<T *>(_address) = value;

			if (UseProtect)
				sm::internal::protect(_address, _size, _protect);
		}

		template<typename T, bool UseProtect = true, typename ContainerT = sm_container_t<T>, typename AddressT>
		void write_container(AddressT address, const ContainerT &value) {
			auto size = value.size();

			const auto _address = sm_convert<sm_address_t>(address);
			const auto _size = sizeof(T) * size;
			SM_PROTECT _protect;

			if (UseProtect)
				_protect = sm::internal::protect(_address, _size, SM_WRITE_ACCESS);

			for (decltype(size) i = 0; i < size; i++)
				*reinterpret_cast<T *>(_address + (sizeof(T) * i)) = value[i];

			if (UseProtect)
				sm::internal::protect(_address, _size, _protect);
		}

		template<typename T, bool UseProtect = false, typename AddressT>
		T read(AddressT address) {
			const auto _address = sm_convert<sm_address_t>(address);
			const auto _size = sizeof(T);
			SM_PROTECT _protect;

			if (UseProtect)
				_protect = sm::internal::protect(_address, _size, SM_READ_ACCESS);

			auto result = *reinterpret_cast<T *>(_address);

			if (UseProtect)
				sm::internal::protect(_address, _size, _protect);

			return result;
		}

		template<typename T, bool UseProtect = false, typename ContainerT = sm_container_t<T>, typename AddressT>
		ContainerT read_container(AddressT address, sm_size_t size) {
			auto result = ContainerT(size);
			
			const auto _address = sm_convert<sm_address_t>(address);
			const auto _size = sizeof(T) * size;
			SM_PROTECT _protect;

			if (UseProtect)
				_protect = sm::internal::protect(_address, _size, SM_READ_ACCESS);

			for (decltype(size) i = 0; i < size; i++)
				result[i] = *reinterpret_cast<T *>(_address + (sizeof(T) * i));
			
			if(UseProtect)
				sm::internal::protect(_address, _size, _protect);

			return result;
		}
	}
}

#endif