//////////////////////////////////////////////////////////////////////////////////
//              Title:  DirectX12Utility.hpp
//            Content:  DirectX12 Helper
//             Author:  Toide Yutaro
//             Create:  2020_11_10
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_DEBUG_HPP
#define DIRECTX12_DEBUG_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <stdexcept>
#include <d3d12.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         HResult Exception Class
//////////////////////////////////////////////////////////////////////////////////
inline std::string HResultToString(HRESULT hresult)
{
	char s_str[64] = {};
	sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hresult));
	return std::string(s_str);
}

class HResultException : public std::runtime_error
{
public:
	HResultException(HRESULT hresult) : std::runtime_error(HResultToString(hresult)), _hresult(hresult) {}
	HRESULT Error() const { return _hresult; }
private:
	const HRESULT _hresult;
};

#define SAFE_RELEASE(p) {if(p){(p)->Release(); p = nullptr;}}


//////////////////////////////////////////////////////////////////////////////////
//                         ThrowIfFailed
//                Always use this function when hresult is returned 
//////////////////////////////////////////////////////////////////////////////////
inline void ThrowIfFailed(HRESULT hresult)
{
	if (FAILED(hresult))
	{
		throw HResultException(hresult);
	}
}


#endif
