
# SimpleMemory++
SimpleMemory++ is a fast and light memory manipulation tool with all features in just one header file.

## Functions
### General
```cpp
template<typename ToT, typename FromT>
constexpr ToT sm_convert(FromT value);
```
### Internal
```cpp
sm_dword_t process_id();

sm_handle_t process();

sm_address_t module_address(const char *moduleName);

sm_address_t base_address();

SM_PROTECT protect(sm_address_t address, sm_size_t size, SM_PROTECT protect);

template<typename T, bool UseProtect = true, typename AddressT>
void write(AddressT address, const T &value);

template<typename T, bool UseProtect = true, typename ContainerT = sm_container_t<T>, typename AddressT>
void write_container(AddressT address, const ContainerT &value);

template<typename T, bool UseProtect = false, typename AddressT>
T read(AddressT address);

template<typename T, bool UseProtect = false, typename ContainerT = sm_container_t<T>, typename AddressT>
ContainerT read_container(AddressT address, sm_size_t size);
```

## Examples
### Read
```cpp
#include "SimpleMemory.hpp"

void ReadInternalMemory1() {
	const auto address = sm::internal::base_address() + 0x10;
	auto value = sm::internal::read<char>(address);
}

void ReadInternalMemory2() {
	const auto address = sm::internal::base_address() + 0x20;
	auto value = sm::internal::read_container<char>(address, 100);
}

struct TestRIM3
{
	int A, B;
	float C;
};
void ReadInternalMemory3() {
	const auto address = sm::internal::base_address() + 0x1000;
	auto value = sm::internal::read<TestRIM3>(address);
}
```
### Write
```cpp
#include "SimpleMemory.hpp"

void WriteInternalMemory1() {
	const auto address = sm::internal::base_address() + 0x10;
	sm::internal::write<char>(address, 'A');
}

void WriteInternalMemory2() {
	const auto address = sm::internal::base_address() + 0x20;
	sm::internal::write_container<char>(address, { 'N', 'I', 'C', 'E' });
}

struct TestWIM3
{
	int A, B;
	float C;
};
void WriteInternalMemory3() {
	const auto address = sm::internal::base_address() + 0x1000;
	TestWIM3 value;
	value.A = 5;
	value.B = 6;
	value.C = 20.5f;
	sm::internal::write<TestWIM3>(address, value);
}
```

## Future Features
 - [ ] External process memory manipulation
 - [ ] Memory backup/restore