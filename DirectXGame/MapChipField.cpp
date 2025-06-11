#include "MapChipField.h"
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>

namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}

	if (yIndex < 0 || kNumBlockVertical - 1 < yIndex) {
		return MapChipType::kBlank;
	}

    return mapChipData_.data[yIndex][xIndex];
}

IndexSet MapChipField::GetMapChipINdexSetByPosition(const Vector3 position) {
	IndexSet indexSet;
	indexSet.xIndex = static_cast<uint32_t>(position.x / kBlockWidth);
	indexSet.yIndex = static_cast<uint32_t>(position.y / kBlockHeight);
	return indexSet;
}

void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear(); // 既存のデータをクリア
	mapChipData_.data.resize(kNumBlockVertical);
	for (std::vector<MapChipType>& mapChipLine : mapChipData_.data) {
		mapChipLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	ResetMapChipData();

	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	std::stringstream mapChipCsv;

	mapChipCsv << file.rdbuf();

	file.close();

	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		std::string line;
		getline(mapChipCsv, line);
		std::stringstream lineStream(line);
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			getline(lineStream, word, ',');
			if (mapChipTable.contains(word)) {
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}
	}
}

Vector3 MapChipField::GetMapPositionTypeByIndex(uint32_t xIndex, uint32_t yIndex) { return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVertical - 1 - yIndex), 0); }
