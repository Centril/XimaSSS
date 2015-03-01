#pragma once

class CGdiPlusBitmap
{
protected:
	Gdiplus::Bitmap* m_pBitmap;

public:
	CGdiPlusBitmap() : m_pBitmap(0)
	{
	}

	CGdiPlusBitmap(const WCHAR* pFile) : m_pBitmap(0)
	{
		this->load(pFile);
	}

	virtual ~CGdiPlusBitmap()
	{ 
		this->empty();
	}

	void empty()
	{
		delete this->m_pBitmap;
		m_pBitmap = 0;
	}

	bool load(const WCHAR* pFile)
	{
		this->empty();
		m_pBitmap = Gdiplus::Bitmap::FromFile(pFile);
		return m_pBitmap->GetLastStatus() == Gdiplus::Ok;
	}

	operator Gdiplus::Bitmap*() const
	{
		return m_pBitmap;
	}
};


class CGdiPlusBitmapResource : public CGdiPlusBitmap
{
protected:
	HGLOBAL m_hBuffer;

public:
	CGdiPlusBitmapResource() : m_hBuffer(0)
	{
	}

	CGdiPlusBitmapResource(const char* pName, const char* pType = RT_RCDATA, HMODULE hInst = 0)
	{
		this->m_hBuffer = 0;
		this->load(pName, pType, hInst);
	}

	CGdiPlusBitmapResource(const unsigned int &id, const char* pType = RT_RCDATA, HMODULE hInst = 0)
	{
		this->m_hBuffer = 0;
		this->load(id, pType, hInst);
	}

	CGdiPlusBitmapResource(const unsigned int &id, const unsigned int &type, HMODULE hInst = 0)
	{
		this->m_hBuffer = 0;
		this->load(id, type, hInst);
	}

	virtual ~CGdiPlusBitmapResource()
	{
		this->empty();
	}

	void empty();

	bool load(const char* pName, const char* pType = RT_RCDATA, HMODULE hInst = 0);
	bool load(const unsigned int &id, const char* pType = RT_RCDATA, HMODULE hInst = 0)
	{
		return this->load(MAKEINTRESOURCE(id), pType, hInst);
	}

	bool load(const unsigned int &id, const unsigned int &type, HMODULE hInst = 0)
	{
		return this->load(MAKEINTRESOURCE(id), MAKEINTRESOURCE(type), hInst);
	}
};

inline void CGdiPlusBitmapResource::empty()
{
	CGdiPlusBitmap::empty();
	if(this->m_hBuffer)
	{
		::GlobalUnlock(this->m_hBuffer);
		::GlobalFree(this->m_hBuffer);
		this->m_hBuffer = 0;
	} 
}

inline bool CGdiPlusBitmapResource::load(const char* pName, const char* pType, HMODULE hInst)
{
	this->empty();

	HRSRC hResource = ::FindResource(hInst, pName, pType);
	if(!hResource)
		return false;
	
	DWORD imageSize = ::SizeofResource(hInst, hResource);
	if(!imageSize)
		return false;

	const void* pResourceData = ::LockResource(::LoadResource(hInst, hResource));
	if(!pResourceData)
		return false;

	this->m_hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if(this->m_hBuffer)
	{
		void* pBuffer = ::GlobalLock(this->m_hBuffer);
		if(pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, imageSize);

			IStream* pStream = 0;
			if(::CreateStreamOnHGlobal(this->m_hBuffer, 0, &pStream) == S_OK)
			{
				this->m_pBitmap = Gdiplus::Bitmap::FromStream(pStream);
				pStream->Release();
				if(this->m_pBitmap)
				{ 
					if(this->m_pBitmap->GetLastStatus() == Gdiplus::Ok)
						return true;

					delete m_pBitmap;
					m_pBitmap = 0;
				}
			}
			::GlobalUnlock(this->m_hBuffer);
		}
		::GlobalFree(this->m_hBuffer);
		this->m_hBuffer = 0;
	}
	return false;
}
