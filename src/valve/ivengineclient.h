#pragma once
#include "../util/memory.h"

#include "cvector.h"
#include "cmatrix.h"
#include "ispatialquery.h"

typedef struct player_info_struct
{
	uint64_t version;
	union
	{
		uint64_t xuid;
		struct
		{
			uint32_t xuidLow;
			uint32_t xuidHigh;
		};
	};
	char name[128];
	int userId;
	char guid[33];
	uint32_t friendsId;
	char friendsName[128];
	bool fakeplayer;
	bool hltv;
	int customfiles[4];
	unsigned char filesdownloaded;
	int entityIndex;
} player_info_struct;

class IVEngineClient
{
public:
	constexpr void GetScreenSize(std::int32_t& width, std::int32_t& height) noexcept
	{
		memory::Call<void>(this, 5, std::ref(width), std::ref(height));
	}

	// returns index of local player
	constexpr std::int32_t GetLocalPlayerIndex() noexcept 
	{
		return memory::Call<std::int32_t>(this, 12);
	}

	constexpr void SetViewAngles(const CVector& viewAngles) noexcept 
	{
		memory::Call<void, const CVector&>(this, 19, viewAngles);
	}

	// returns true if in-game
	constexpr bool IsInGame() noexcept 
	{
		return memory::Call<bool>(this, 26);
	}

	constexpr const CMatrix4x4& WorldToScreenMatrix() noexcept
	{
		return memory::Call<const CMatrix4x4&>(this, 37);
	}

	constexpr ISpacialQuery* GetBSPTreeQuery() noexcept
	{
		return memory::Call<ISpacialQuery*>(this, 43);
	}

	constexpr auto getPlayerInfo(int entityIndex, const player_info_struct& playerInfo) noexcept
	{
		return memory::Call<bool, int, const player_info_struct&>(this, 8, entityIndex, playerInfo);
	}

};
