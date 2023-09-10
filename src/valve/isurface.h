#pragma once
#include "../util/memory.h"


enum EFontDrawType : std::int32_t
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE
};

enum EFontFlag
{
	FONT_FLAG_NONE,
	FONT_FLAG_ITALIC = 0x001,
	FONT_FLAG_UNDERLINE = 0x002,
	FONT_FLAG_STRIKEOUT = 0x004,
	FONT_FLAG_SYMBOL = 0x008,
	FONT_FLAG_ANTIALIAS = 0x010,
	FONT_FLAG_GAUSSIANBLUR = 0x020,
	FONT_FLAG_ROTARY = 0x040,
	FONT_FLAG_DROPSHADOW = 0x080,
	FONT_FLAG_ADDITIVE = 0x100,
	FONT_FLAG_OUTLINE = 0x200,
	FONT_FLAG_CUSTOM = 0x400,
	FONT_FLAG_BITMAP = 0x800,
};

class ISurface
{
public:
	constexpr void DrawSetColor(int r, int g, int b, int a) noexcept
	{
		memory::Call<void>(this, 15, r, g, b, a);
	}

	constexpr void DrawFilledRect(int x, int y, int xx, int yy) noexcept
	{
		memory::Call<void>(this, 16, x, y, xx, yy);
	}

	// to get the index we count the amount of virtual functions in the ISurface file in the 2018 csgo source. Make sure to include ALL other inherited classes. 
	// Start from 0 each class and then add both results.
	constexpr void DrawOutlinedRect(int x, int y, int xx, int yy) noexcept
	{
		memory::Call<void>(this, 18, x, y, xx, yy);
	}

	unsigned long CreateFont1() noexcept
	{
		return memory::Call<unsigned long>(this, 71);
	}

	constexpr void DrawSetTextColor(int r, int g, int b, int a) noexcept
	{
		memory::Call<void>(this, 25, r, g, b, a);
	}

	constexpr void DrawSetTextPos(int x, int y) noexcept
	{
		memory::Call<void>(this, 26, x, y);
	}

	constexpr void DrawPrintText(const wchar_t* text, int textLen, EFontDrawType drawtype = EFontDrawType::FONT_DRAW_DEFAULT) noexcept
	{
		memory::Call<void>(this, 28, text, textLen, drawtype);
	}

	void DrawSetTextFont(unsigned long font) noexcept
	{
		memory::Call<void>(this, 23, font);
	}

	bool SetFontGlyphSet(unsigned long font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0) noexcept
	{
		return memory::Call<bool>(this, 72, font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax);
	}

	constexpr void DrawOutlinedCircle(int x, int y, int radius, int segments) noexcept
	{
		memory::Call<void>(this, 103, x, y, radius, segments);
	}
};