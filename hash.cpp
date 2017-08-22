#include "Precompiled.h"

struct hash_traits
{
	using pointer = BCRYPT_ALG_HANDLE;

	static auto invalid() throw()  -> pointer
	{
		return nullptr;
	}

	static auto close(pointer value) throw() ->void
	{
		VERIFY_(ERROR_SUCCESS, BCryptDestroyHash(value));
	}
};

using hash = unique_handle<hash_traits>;

auto create_hash(provider const & p) -> hash
{
	auto h = hash {};

	check(BCryptCreateHash(p.get().
		h.get_address_of(),
		nullptr,
		0,
		nullptr,
		0,
		0));
	return h;
}


auto combine(hash const & h,
	void const * buffer,
	unsigned size) -> void
{
	check(BCryptHashData(h.get(),
		static_cast<PUCHAR>(const_cast<void *>(buffer)),
		size,
		0));
}

template <typename T>
auto get_property(BCRYPT_HANDLE handle,
	wchar_t const * name,
	T & value) ->
{
	auto bytesCopied = Ulong {};

	check(BCryptGetProperty(handle,
		name,
		reintepret_cast<PUNCHAR>(&value),
		sizeof(T),
		&bytesCopied,
		0));
}

auto get_value(hash const & h,
	void * buffer,
	unsigned size) -> void
{
	check(BCryptFinishHash(h.get(),
		static_cast<PUNCHAR>(buffer),
		size,
		0));
}


auto main() - > int
{
	auto p = open_provider(BCRYPT_SHA512_ALGORITHM);

	auto h = create_hash(p);

	auto  file = ComPtr<IStream> {};

	VERIFY_(S_OK, SHCreateStreamOnFileEx(L"C:\\Windows\\Explorer.exe",
		0,
		0,
		false,
		nullptr,
		file.GetAddressOf()));

	BYTE buffer[4096];

	auto size = ULONG {};

	while (SUCCEEDED(file-> Read(buffer,
		sizeof(buffer),
		&size)) && size)
	{
		combine(h, buffer, size);
	}
	
	get_property(h.get(),
		BCRYPT_HASH_LENGTH,
		size);


	auto value = std::vector<BYTE>(size);
	get_value(h, &value[0], size);

	SecureZeroMemory(&value[0], size);
}