#pragma once  
#include <cstdint>  
#include <vector>  
#include <KamataEngine.h>

using namespace KamataEngine;

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};  

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
public:
	MapChipData mapChipData_;                                            // マップチップデータ
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex); // 指定したインデックスのマップチップデータを取得する
	void ResetMapChipData();                                             // マップチップデータのリセット
	void LoadMapChipCsv(const std::string& filePath);                    // マップチップデータの読み込み
	Vector3 GetMapPositionTypeByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	static inline const float kBlockWidth = 1.0f;          // ブロック1個分の横幅
	static inline const float kBlockHeight = 1.0f;         // ブロック1個分の縦幅
	static inline const uint32_t kNumBlockVertical = 20;   // 縦のブロック数
	static inline const uint32_t kNumBlockHorizontal = 20; // 横のブロック数
};