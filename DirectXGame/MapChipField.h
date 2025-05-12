#pragma once
#include <cstdint>
#include <vector>

class MapChipField {
	static inline const float kBlockWidth = 1.0f; // ブロック1個分の横幅
	static inline const float kBlockHeight = 1.0f; // ブロック1個分の縦幅
	static inline const uint32_t kNumBlockVertical = 20; // 縦のブロック数
	static inline const uint32_t kNumBlockHorizontal = 20; // 横のブロック数
	MapChipData mapChipData_;                              // マップチップデータ
	void ResetMapChipData();                               // マップチップデータのリセット
	void LoadMapChipCsv(const std::string& filePath);     // マップチップデータの読み込み
};

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};