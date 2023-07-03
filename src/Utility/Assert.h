// Copyright (c) 2023 Sebastian Ploch
#pragma once

#include <exception>

// Taken from https://github.com/microsoft/DirectXTK/wiki/ThrowIfFailed

namespace AssertInternal
{
	class COMException final : public std::exception
	{
	public:
		COMException() = delete;
		COMException(HRESULT HR) : result(HR) {};

		const char* what() const noexcept override
		{
			return std::format("Failure with HRESULT of {}", static_cast<unsigned int>(result)).c_str();
		}

	private:
		HRESULT result;
	};
}

inline void ThrowIfFailed(HRESULT HR)
{
	if (FAILED(HR))
	{
		throw AssertInternal::COMException(HR);
	}
}