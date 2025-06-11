#pragma once
#include <KamataEngine.h>
#include <cstdint>
#include <vector>

using namespace KamataEngine;

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

struct IndexSet {
	uint32_t xIndex;
	uint32_t yIndex;
};

struct Rect {
	float left;
	float right;
	float bottom;
	float top;
};

class MapChipField {
public:
	MapChipData mapChipData_;
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	IndexSet GetMapChipIndexSetByPosition(const Vector3 position); // ← IndexSet
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	Vector3 GetMapPositionTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetNumBlockVertical() const { return kNumBlockVertical; } // ← Vertical
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	static inline const uint32_t kNumBlockVertical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;
};