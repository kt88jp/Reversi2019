#pragma once
#include <stdint.h>
#include<vector>

using namespace std;
class Field
{
public:
	

	int field[8][8];
	//vector<vector<int>> field;

	//Field():field(8, vector<int>(8)) {}

	int numOfEachPiece[3];

	void coutNumOfEachPieces() {

		for (auto& i : numOfEachPiece) {
			i = 0;
		}
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				numOfEachPiece[field[y][x]]++;
			}
		}
	}


	bool isInsideOfTheField(const int x, const int y) {
		if (x < 0 || x >= 8 || y < 0 || y >= 8) {
			return false;
		}
		else {
			return true;
		}
	}

	bool canPutOrNot(const int x, const int y, const bool isReverse, const int turn) {
		int dx[8] = { -1, -1, 0, +1, +1, +1, 0, -1 };
		int dy[8] = { 0, -1, -1, -1, 0, +1, +1, +1 };

		bool result = false;

		int checkX, checkY;

		if (field[y][x] != 0) {
			return false;
		}

		for (int i = 0; i < 8; i++) {
			checkX = x + dx[i];
			checkY = y + dy[i];

			if (!isInsideOfTheField(checkX, checkY)) {
				continue;
			}

			if (field[checkY][checkX] == 0 || field[checkY][checkX] == turn) {
				continue;
			}

			do {
				checkX += dx[i];
				checkY += dy[i];
				if (!isInsideOfTheField(checkX, checkY)) {
					break;
				}
				if (field[checkY][checkX] == 0) {
					break;
				}
				else if (field[checkY][checkX] == turn) {
					result = true;
					int reverseX = x;
					int reverseY = y;

					if (isReverse) {
						while (reverseX != checkX || reverseY != checkY) {
							field[reverseY][reverseX] = turn;
							reverseX += dx[i];
							reverseY += dy[i];
						}
						coutNumOfEachPieces();
					}
				}
			} while (true);
		}
		return result;
	}

	double howManyStones(int x, int y, int turn) {
		int vfield[8][8];
		memcpy(vfield, field, sizeof(int) * 8 * 8);
		//vector<vector<int>> vfield = field;

		int dx[8] = { -1, -1, 0, +1, +1, +1, 0, -1 };
		int dy[8] = { 0, -1, -1, -1, 0, +1, +1, +1 };

		int cnt = 0;
		bool isReverse = true;

		int checkX, checkY;

		if (field[y][x] != 0) {
			return false;
		}

		for (int i = 0; i < 8; i++) {
			checkX = x + dx[i];
			checkY = y + dy[i];

			if (!isInsideOfTheField(checkX, checkY)) {
				continue;
			}

			if (vfield[checkY][checkX] == 0 || vfield[checkY][checkX] == turn) {
				continue;
			}

			do {
				checkX += dx[i];
				checkY += dy[i];
				if (!isInsideOfTheField(checkX, checkY)) {
					break;
				}
				if (vfield[checkY][checkX] == 0) {
					break;
				}
				else if (vfield[checkY][checkX] == turn) {
					int reverseX = x + dx[i];
					int reverseY = y + dy[i];

					if (isReverse) {
						while (reverseX != checkX || reverseY != checkY) {
							vfield[reverseY][reverseX] = turn;
							cnt++;
							reverseX += dx[i];
							reverseY += dy[i];
						}
					}
					break;
				}
			} while (true);
		}
		return static_cast<double>(cnt);

	}

	double calcOpenness(int x, int y, int turn) {
		int vfield[8][8];
		memcpy(vfield, field, sizeof(int) * 8 * 8);
		//vector<vector<int>> vfield = field;

		int dx[8] = { -1, -1, 0, +1, +1, +1, 0, -1 };
		int dy[8] = { 0, -1, -1, -1, 0, +1, +1, +1 };

		int cnt = 0;
		bool isReverse = true;

		int checkX, checkY;

		/*if (field[y][x] != 0) {
			return -INF;
		}*/

		for (int i = 0; i < 8; i++) {
			checkX = x + dx[i];
			checkY = y + dy[i];

			if (!isInsideOfTheField(checkX, checkY)) {
				continue;
			}

			if (vfield[checkY][checkX] == 0 || vfield[checkY][checkX] == turn) {
				continue;
			}

			do {
				checkX += dx[i];
				checkY += dy[i];
				if (!isInsideOfTheField(checkX, checkY)) {
					break;
				}
				if (vfield[checkY][checkX] == 0) {
					break;
				}
				else if (vfield[checkY][checkX] == turn) {
					int reverseX = x + dx[i];
					int reverseY = y + dy[i];

					if (isReverse) {
						while (reverseX != checkX || reverseY != checkY) {
							vfield[reverseY][reverseX] = turn;
							for (int j = 0; j < 8; j++){
								if (isInsideOfTheField(reverseX + dx[j], reverseY + dy[j])) {
									if (field[reverseY + dy[j]][reverseX + dx[j]] == 0) {
										cnt--;
									}
								}
								
							}
							reverseX += dx[i];
							reverseY += dy[i];
						}
					}
					break;
				}
			} while (true);
		}
		return static_cast<double>(cnt);

	}




	//fnv1a 32 and 64 bit hash functions
	// key is the data to hash, len is the size of the data (or how much of it to hash against)
	// code license: public domain or equivalent
	// post: https://notes.underscorediscovery.com/constexpr-fnv1a/

	inline const uint32_t hash_32_fnv1a(const void* key, const uint32_t len) {

		const char* data = (char*)key;
		uint32_t hash = 0x811c9dc5;
		uint32_t prime = 0x1000193;

		for (int i = 0; i < len; ++i) {
			uint8_t value = data[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;

	} //hash_32_fnv1a

	inline const uint64_t hash_64_fnv1a(const void* key, const uint64_t len) {

		const char* data = (char*)key;
		uint64_t hash = 0xcbf29ce484222325;
		uint64_t prime = 0x100000001b3;

		for (int i = 0; i < len; ++i) {
			uint8_t value = data[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;

	} //hash_64_fnv1a


	uint64_t calc_hash() {
		return hash_64_fnv1a(this, sizeof(Field));
	}
};

