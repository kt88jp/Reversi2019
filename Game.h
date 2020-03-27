#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <random>
#include <locale>
#include "stopwatch.h"
#include "Field.h"
#include <stdint.h>
#include "tt.h"

constexpr double INF = std::numeric_limits<double>::infinity();

using namespace std;





int turn = 1;
bool isGameRunning = true;

class Game {
public:

	enum EvalType {
		HUMAN,
		RANDOM,
		GREEDY,
		FIELDEVALUATION,
		PUTNUMPRIORITY,
		OPENNESS
	};

	enum SearchType {
		NONE,
		DEPTHFIRST,
		BEAM,
	};

	TT_TABLE tt;


	//EvalType aitype[2] = { EvalType::HUMAN, EvalType::PUTNUMPRIORITY };
	EvalType evaltype[2] = { EvalType::HUMAN, EvalType::HUMAN };
	SearchType searchtype[2] = { SearchType::NONE, SearchType::NONE };
	int waitcounter = 0;
	int waittime = 5;
	bool isWait = true;

	bool skip = false;

	int win = 0;
	int lose = 0;
	int drawNum = 0;

	static constexpr double FINISHEVAL = 10000;

	Field f;

	int autobattle = 0;
	int autobattlecnt = 0;
	unique_ptr<Stopwatch> sw;

	int AIdepth[2] = { 3,3 };
	int AIwidth[2] = { 100, 100 };

	pair<int, int> bestmove;

	int searchcnt[2];

	int emptyNum;

	//Stopwatch sw;

	Game() {
		 sw = Stopwatch::startNew();
	}

	unsigned int key;

	int bOrw = 0;

	bool isdfs;

	enum curMode {
		GAME,
		OPTIONAI,
		OPTIONEVAL,
		OPTIONSEARCH,
		OPTIONDEPTH,
		OPTIONWIDTH
	};

	curMode curMode;

	void keyfunc(unsigned int keycode) {
		key = keycode;
		if (keycode == 'B') {
			bOrw = 0;
			return;
		}

		if (keycode == 'W') {
			bOrw = 1;
			return;
		}

		if (keycode == 27) {
			curMode = curMode::GAME;
		}
		switch(curMode) {
		case curMode::GAME:
			if (keycode == 'A') {
				curMode = curMode::OPTIONAI;
			}
			break;
		case curMode::OPTIONAI:
			switch (keycode) {
			case '1':
				//人間
				evaltype[bOrw] = EvalType::HUMAN;
				curMode = curMode::GAME;
				searchtype[bOrw] = SearchType::NONE;
				break;
			case '2':
				//AI
				curMode = curMode::OPTIONEVAL;
				break;
			case 8:
				curMode = curMode::GAME;
				break;
			}
			break;
		case curMode::OPTIONEVAL:
			switch (keycode) {
			case '1':
				//ランダム
				evaltype[bOrw] = EvalType::RANDOM;
				curMode = curMode::GAME;
				searchtype[bOrw] = SearchType::NONE;
				break;
			case '2':
				//開放度
				evaltype[bOrw] = EvalType::OPENNESS;
				curMode = curMode::GAME;
				searchtype[bOrw] = SearchType::NONE;
				break;
			case '3':
				//盤面評価
				evaltype[bOrw] = EvalType::FIELDEVALUATION;
				curMode = curMode::GAME;
				searchtype[bOrw] = SearchType::NONE;
				break;
			case '4':
				//～手後の置ける数
				evaltype[bOrw] = EvalType::PUTNUMPRIORITY;
				curMode = curMode::OPTIONSEARCH;
				break;
			case '5':
				//貪欲
				evaltype[bOrw] = EvalType::GREEDY;
				curMode = curMode::OPTIONSEARCH;
				break;
			case 8:
				curMode = curMode::OPTIONAI;
				break;
			}
			break;
		case curMode::OPTIONSEARCH:
			switch (keycode) {
			case '1':
				//dfs
				isdfs = true;
				curMode = curMode::OPTIONDEPTH;
				searchtype[bOrw] = SearchType::DEPTHFIRST;
				break;
			case '2':
				//beam
				isdfs = false;
				searchtype[bOrw] = SearchType::BEAM;
				curMode = curMode::OPTIONDEPTH;
				break;
			case '3':
				searchtype[bOrw] = SearchType::NONE;
				curMode = curMode::GAME;
				break;
			case 8:
				curMode = curMode::OPTIONEVAL;
				break;
			}
			break;
		case curMode::OPTIONDEPTH:
			if (keycode - '0' <= 5) {
				AIdepth[bOrw] = keycode - '0';
			}

			if (keycode == 8) {
				curMode = curMode::OPTIONSEARCH;
			}

			if (!isdfs) {
				curMode = curMode::OPTIONWIDTH;
			}
			else {
				curMode = curMode::GAME;
			}
			break;
		case curMode::OPTIONWIDTH:
			if (keycode - '0' <= 9 ) {
				AIwidth[bOrw] = (keycode - '0') * 100;
			}
			if (keycode == 8) {
				curMode = curMode::OPTIONDEPTH;
			}

			curMode = curMode::GAME;
			break;
		}
	}

	void drawGameInfo(const HDC& hdc) const {
		// 開放の方法は DeleteObject(hBrush);
		static HBRUSH whiteb = CreateSolidBrush(RGB(255, 255, 255));
		static HBRUSH blackb = CreateSolidBrush(RGB(0, 0, 0));
		static HBRUSH greenb = CreateSolidBrush(RGB(108, 173, 119));
		HBRUSH hdefb;


		hdefb = (HBRUSH)SelectObject(hdc, greenb);
		Rectangle(hdc, 50, 50, 450, 450);

		for (int i = 1; i <= 9; i++) {
			MoveToEx(hdc, 50 * i, 50, nullptr);
			LineTo(hdc, 50 * i, 450);
		}

		for (int i = 1; i <= 9; i++) {
			MoveToEx(hdc, 50, 50 * i, nullptr);
			LineTo(hdc, 450, 50 * i);
		}

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (f.field[i][j] == 1) {
					(HBRUSH)SelectObject(hdc, blackb);
					Ellipse(hdc, 50 + 50 * j, 50 + 50 * i, 100 + 50 * j, 100 + 50 * i);
				}
				else if (f.field[i][j] == 2) {
					(HBRUSH)SelectObject(hdc, whiteb);
					Ellipse(hdc, 50 + 50 * j, 50 + 50 * i, 100 + 50 * j, 100 + 50 * i);
				}
			}
		}
		MoveToEx(hdc, 0, 500, nullptr);
		LineTo(hdc, waitcounter * 10, 500);

		setlocale(LC_ALL, "japanese");

		char txt[100];
		WCHAR lptStr[100];

		sprintf_s(txt, sizeof(txt), "現在、黒が%2d個、白が%2d個", f.numOfEachPiece[1], f.numOfEachPiece[2]);
		convert_wchar(txt, lptStr);
		TextOut(hdc, 10, 30, lptStr, lstrlen(lptStr));

		float ratio;
		if (win + lose + drawNum == 0) {
			ratio = 0;
		}
		else {
			ratio = static_cast<float>(win) / static_cast<float>(win + lose + drawNum);
		}
		int t;
		t = static_cast<int>(sw->getElapsedMilliseconds());

		sprintf_s(txt, sizeof(txt), "現在、黒が%2d勝、%2d敗、%2d引き分け、勝率%0.3f、経過時間%dms", win, lose, drawNum, ratio, t);
		convert_wchar(txt, lptStr);
		TextOut(hdc, 10, 10, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "探索回数、黒：%d回、白：%d回", searchcnt[0], searchcnt[1]);
		convert_wchar(txt, lptStr);
		TextOut(hdc, 250, 30, lptStr, lstrlen(lptStr));


		sprintf_s(txt, sizeof(txt), "押されたキー：%d", key);
		convert_wchar(txt, lptStr);
		TextOut(hdc, 800, 600, lptStr, lstrlen(lptStr));

		(HBRUSH)SelectObject(hdc, hdefb);
		//		case 'A':
	}

	void drawOptionAI(const HDC& hdc) const {

		char txt[100];
		WCHAR lptStr[100];

		if (bOrw == 0) {
			sprintf_s(txt, sizeof(txt), "%s", "黒の設定");
		}
		else {
			sprintf_s(txt, sizeof(txt), "%s", "白の設定");
		}
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 10, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "人間かAIかを選択してください");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 30, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "１：人間");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 50, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "２：AI");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 70, lptStr, lstrlen(lptStr));

	}

	void drawOptionEval(const HDC& hdc) const {

		char txt[100];
		WCHAR lptStr[100];

		if (bOrw == 0) {
			sprintf_s(txt, sizeof(txt), "%s", "黒の設定");
		}
		else {
			sprintf_s(txt, sizeof(txt), "%s", "白の設定");
		}
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 10, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "評価関数を選択してください");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 30, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "１：ランダム");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 50, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "２：開放度");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 70, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "３：盤面評価");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 90, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "４：置ける数");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 110, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "５：自分の石の総数");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 130, lptStr, lstrlen(lptStr));

	}

	void drawOptionSearch(const HDC& hdc) const {

		char txt[100];
		WCHAR lptStr[100];

		if (bOrw == 0) {
			sprintf_s(txt, sizeof(txt), "%s", "黒の設定");
		}
		else {
			sprintf_s(txt, sizeof(txt), "%s", "白の設定");
		}
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 10, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "探索アルゴリズムを選択してください");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 30, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "１：Depth-First-Search");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 50, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "２：Beam-Search");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 70, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "３：探索をしない（その状況で最大を選ぶ）");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 90, lptStr, lstrlen(lptStr));

	}

	void drawOptionDepth(const HDC& hdc) const {

		char txt[100];
		WCHAR lptStr[100];

		if (bOrw == 0) {
			sprintf_s(txt, sizeof(txt), "%s", "黒の設定");
		}
		else {
			sprintf_s(txt, sizeof(txt), "%s", "白の設定");
		}
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 10, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "探索深さを選択してください");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 30, lptStr, lstrlen(lptStr));

		for (int i = 1; i <= 5; i++) {
			sprintf_s(txt, sizeof(txt), "%d", i);
			convert_wchar(txt, lptStr);
			TextOut(hdc, 40, 40 + 20 * i, lptStr, lstrlen(lptStr));
		}


	}

	void drawOptionWidth(const HDC& hdc) const {

		char txt[100];
		WCHAR lptStr[100];

		if (bOrw == 0) {
			sprintf_s(txt, sizeof(txt), "%s", "黒の設定");
		}
		else {
			sprintf_s(txt, sizeof(txt), "%s", "白の設定");
		}
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 10, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "探索幅を選択してください");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 40, 30, lptStr, lstrlen(lptStr));

		for (int i = 1; i <= 9; i++) {
			sprintf_s(txt, sizeof(txt), "%d：%d", i, i * 100);
			convert_wchar(txt, lptStr);
			TextOut(hdc, 40, 40 + 20 * i, lptStr, lstrlen(lptStr));
		}


	}

	void drawCurrentInfo(const HDC& hdc) const {
		char txt[100];
		WCHAR lptStr[100];

		sprintf_s(txt, sizeof(txt), "%s", "-現在の設定-");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 30, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "黒");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 50, lptStr, lstrlen(lptStr));


		if (evaltype[0] == EvalType::HUMAN) {
			sprintf_s(txt, sizeof(txt), "%s", "タイプ：人間");
		}
		else {
			sprintf_s(txt, sizeof(txt), "%s", "タイプ：AI");
		}
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 70, lptStr, lstrlen(lptStr));

		switch (evaltype[0]) {
		case EvalType::HUMAN:
			sprintf_s(txt, sizeof(txt), "%s", "評価関数：人間");
			break;
		case EvalType::FIELDEVALUATION:
			sprintf_s(txt, sizeof(txt), "%s", "評価関数：盤面評価");
			break;
		case EvalType::GREEDY:
			sprintf_s(txt, sizeof(txt), "%s", "評価関数：自分の石の総数");
			break;
		case EvalType::OPENNESS:
			sprintf_s(txt, sizeof(txt), "%s", "評価関数：開放度");
			break;
		case EvalType::PUTNUMPRIORITY:
			sprintf_s(txt, sizeof(txt), "%s", "評価関数：置ける数");
			break;
		case EvalType::RANDOM:
			sprintf_s(txt, sizeof(txt), "%s", "評価関数：ランダム");
			break;
		}
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 90, lptStr, lstrlen(lptStr));

		switch (searchtype[0]) {
		case SearchType::BEAM:
			sprintf_s(txt, sizeof(txt), "%s", "探索アルゴリズム：Beam-Search");
			break;
		case SearchType::DEPTHFIRST:
			sprintf_s(txt, sizeof(txt), "%s", "探索アルゴリズム：Depth-First-Search");
			break;
		case SearchType::NONE:
			sprintf_s(txt, sizeof(txt), "%s", "探索アルゴリズム：なし");
			break;
		}
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 110, lptStr, lstrlen(lptStr));


		sprintf_s(txt, sizeof(txt), "%s%d", "深さ：", AIdepth[0]);
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 130, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s%d", "幅：", AIwidth[0]);
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 150, lptStr, lstrlen(lptStr));





		sprintf_s(txt, sizeof(txt), "%s", "白");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 190, lptStr, lstrlen(lptStr));


		if (evaltype[1] == EvalType::HUMAN) {
			sprintf_s(txt, sizeof(txt), "%s", "タイプ：人間");
		}
		else {
			sprintf_s(txt, sizeof(txt), "%s", "タイプ：AI");
		}
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 210, lptStr, lstrlen(lptStr));

		switch (evaltype[1]) {
		case EvalType::HUMAN:
			sprintf_s(txt, sizeof(txt), "%s", "評価関数：人間");
			break;
		case EvalType::FIELDEVALUATION:
			sprintf_s(txt, sizeof(txt), "%s", "評価関数：盤面評価");
			break;
		case EvalType::GREEDY:
			sprintf_s(txt, sizeof(txt), "%s", "評価関数：自分の石の総数");
			break;
		case EvalType::OPENNESS:
			sprintf_s(txt, sizeof(txt), "%s", "評価関数：開放度");
			break;
		case EvalType::PUTNUMPRIORITY:
			sprintf_s(txt, sizeof(txt), "%s", "評価関数：置ける数");
			break;
		case EvalType::RANDOM:
			sprintf_s(txt, sizeof(txt), "%s", "評価関数：ランダム");
			break;
		}
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 230, lptStr, lstrlen(lptStr));

		switch (searchtype[1]) {
		case SearchType::BEAM:
			sprintf_s(txt, sizeof(txt), "%s", "探索アルゴリズム：Beam-Search");
			break;
		case SearchType::DEPTHFIRST:
			sprintf_s(txt, sizeof(txt), "%s", "探索アルゴリズム：Depth-First-Search");
			break;
		case SearchType::NONE:
			sprintf_s(txt, sizeof(txt), "%s", "探索アルゴリズム：なし");
			break;
		}
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 250, lptStr, lstrlen(lptStr));


		sprintf_s(txt, sizeof(txt), "%s%d", "深さ：", AIdepth[1]);
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 270, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s%d", "幅：", AIwidth[1]);
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 290, lptStr, lstrlen(lptStr));


		if (bOrw == 0) {
			sprintf_s(txt, sizeof(txt), "%s", "現在選択中の色：黒");
		}
		else {
			sprintf_s(txt, sizeof(txt), "%s", "現在設定中の色：白");
		}
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 330, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "-設定のやり方-");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 370, lptStr, lstrlen(lptStr));


		sprintf_s(txt, sizeof(txt), "%s", "Wキー：白を選択、Bキー黒を選択、Aキー：設定画面へ");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 390, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "Escキー：設定画面から抜ける（変更は保存されない）");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 410, lptStr, lstrlen(lptStr));

		sprintf_s(txt, sizeof(txt), "%s", "BackSpace：（設定画面で）一つ戻る");
		convert_wchar(txt, lptStr);
		TextOut(hdc, 550, 430, lptStr, lstrlen(lptStr));




	}



	void draw(const HDC& hdc) const {

		drawCurrentInfo(hdc);

		switch (curMode) {
		case curMode::GAME:
			drawGameInfo(hdc);
			break;
		case curMode::OPTIONAI:
			drawOptionAI(hdc);
			break;
		case curMode::OPTIONEVAL:
			drawOptionEval(hdc);
			break;
		case curMode::OPTIONSEARCH:
			drawOptionSearch(hdc);
			break;
		case curMode::OPTIONDEPTH:
			drawOptionDepth(hdc);
			break;
		case curMode::OPTIONWIDTH:
			drawOptionWidth(hdc);
			break;
		}
		
	}

	static void convert_wchar(const char* txt, WCHAR* lptStr) {
		size_t s;
		mbstowcs_s(&s, lptStr, strlen(txt) + 1, txt, _TRUNCATE);
	}
	void mousedown(const int x, const int y) {
		int pointX = (x - 50) / 50;
		int pointY = (y - 50) / 50;

		if (x < 50 || y < 50) return;

		if (!f.isInsideOfTheField(pointX, pointY)) {
			return;
		}

		if (f.field[pointY][pointX] == 0) {
			if (f.canPutOrNot(pointX, pointY, true, turn)) {
				emptyNum--;
				f.field[pointY][pointX] = turn;
				waitcounter = waittime;
				changeTurn();
				isGameOver();
			}
		}

		invalidate();

	}

	void isGameOver() {
		if (isPass()) {
			changeTurn();
			if (isPass()) {
				isGameRunning = false;
				f.coutNumOfEachPieces();
				if (f.numOfEachPiece[1] > f.numOfEachPiece[2]) {
					win++;
				}
				else if (f.numOfEachPiece[1] < f.numOfEachPiece[2]) {
					lose++;
				}
				else {
					drawNum++;
				}
			}
		}
	}

	void changeTurn() {
		if (turn == 1) {
			turn = 2;
		} else {
			turn = 1;
		}

		tt.clear();
	}

	bool isPass() {
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				if (f.field[y][x] == 0) {
					if (f.canPutOrNot(x, y, false, turn)) {
						return false;
					}
				}
			}
		}
		return true;
	}

	HWND hWnd;

	void sethWnd(const HWND _hWnd) {
		hWnd = _hWnd;
		init();
	}

	void init() {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				f.field[i][j] = 0;
			}
		}

		f.field[3][4] = 1;
		f.field[4][4] = 2;
		f.field[4][3] = 1;
		f.field[3][3] = 2;

		turn = 1;
		emptyNum= 60;

		f.coutNumOfEachPieces();

		isGameRunning = true;

		tt.clear();
	}

	void invalidate() const {
	/*	RECT rc;

		SetRect(&rc, 0, 0, 1000, 1000);*/

		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
	}

	pair<int, int> randomAI() {
		vector<pair<int, int>> canput;

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (f.canPutOrNot(i, j, false, turn)) {
					canput.push_back(make_pair(i, j));
				}
			}
		}

		random_device rnd;     // 非決定的な乱数生成器を生成
		mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
		uniform_int_distribution<> rand100(0, canput.size() - 1);

		int rndnum = rand100(mt);

		return canput[rndnum];
	}

	double greedyAI(Field nowf) {


		random_device rnd;     // 非決定的な乱数生成器を生成
		mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
		uniform_real_distribution<double> rand1(0, 1);


		return nowf.numOfEachPiece[turn] - nowf.numOfEachPiece[3 - turn] + rand1(mt);

	}

	pair<int,int> opennessAi(Field nowf) {
		pair<int, int> res = make_pair(0, 0);
		double ev = -INF;
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				if (nowf.canPutOrNot(x, y, false, turn)) {
					double tmp = nowf.calcOpenness(x, y, turn);
					if (ev < tmp) {
						res = make_pair(x, y);
						ev = tmp;
					}
				}
			}
		}

		return res;

	}

	double canPutNum(Field nowf) {
		double res = 0;
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				if (nowf.canPutOrNot(x, y, false, turn)) {
					res++;
				}
			}
		}
		return res;
	}


	pair<int, int> evaluationAI() {
		constexpr double evaluationtb[8][8] = {
			{ 30,-12,  0, -1, -1,  0,-12, 30},
			{-12,-15, -3, -3, -3, -3,-15,-12},
			{  0, -3,  0, -1, -1,  0, -3,  0},
			{ -1, -3, -1, -1, -1, -1, -3, -1},
			{ -1, -3, -1, -1, -1, -1, -3, -1},
			{  0, -3,  0, -1, -1,  0, -3,  0},
			{-12,-15, -3, -3, -3, -3,-15,-12},
			{ 30,-12,  0, -1, -1,  0,-12, 30},
		};

		vector < pair<double, pair<int, int>>> eval_x_y;

		random_device rnd;     // 非決定的な乱数生成器を生成
		mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
		uniform_real_distribution<double> rand1(0, 1);

		

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (!f.canPutOrNot(i, j, false, turn)) continue;
				double rndnum = rand1(mt);
				eval_x_y.push_back(make_pair(evaluationtb[i][j] + rndnum, make_pair(i, j)));
			}
		}

		int maxIndex = 0;
		double maxi = eval_x_y[0].first;
		for (int i = 0; i < eval_x_y.size(); i++) {
			if (maxi < eval_x_y[i].first) {
				maxi = eval_x_y[i].first;
				maxIndex = i;
			}
		}

		return eval_x_y[maxIndex].second;

	}

	void loop() {
		while (true) {

			waitcounter--;
			if (waitcounter > 0 || isGameRunning == false) {
				return;
			}
			switch (searchtype[turn - 1]) {
			case SearchType::DEPTHFIRST:
				//ab_search(1, AIdepth[turn - 1], turn, turn, f, false, -INF, INF);
				mixAI(0, 0, 0);
				break;
			case SearchType::BEAM:
				beamSearch(AIdepth[turn - 1], AIwidth[turn - 1], f, turn, turn, evaltype[turn - 1]);
				break;
			case SearchType::NONE:
				switch (evaltype[turn - 1]) {
				case EvalType::HUMAN:
					return;
				case EvalType::RANDOM:
					bestmove = randomAI();
					break;
				case EvalType::GREEDY:
					ab_search(1, AIdepth[turn - 1], turn, turn, f, false, -INF, INF);
				case EvalType::OPENNESS:
					bestmove = opennessAi(f);
					break;
				case EvalType::PUTNUMPRIORITY:
					ab_search(1, AIdepth[turn - 1], turn, turn, f, false, -INF, INF);
					break;
				case EvalType::FIELDEVALUATION:
					bestmove = evaluationAI();
					break;
				//default:
					//double result = df_search(1, turn, turn, f);
					//double alpha = -INF;
					//double beta = INF;
					//double result = ab_search(1, turn, turn, f, false, -INF, INF);
					//beamSearch(5, 100, f, turn, turn, evaltype[turn]);
				}
			}
			

			if (f.canPutOrNot(bestmove.first, bestmove.second, false, turn)) {
				f.canPutOrNot(bestmove.first, bestmove.second, true, turn);
				emptyNum--;
			}
			else {
				int a = 1;
			}
			changeTurn();
			isGameOver();

			if (isWait) {
				waitcounter = waittime;
			}
			
			if (!isGameRunning || !skip) {
				autobattlecnt++;
				if (autobattle > autobattlecnt) {
					init();
				}
				else {
					sw->stop();
					break;
				}
				
			}
			
		}
		
	}



	void startAutobattle(int num, HMENU hmn) {
		win = 0;
		lose = 0;
		drawNum = 0;
		autobattle = num;
		autobattlecnt = 0;
		skip = true;
		CheckMenuItem(hmn, ID_32780, MFS_UNCHECKED);
		waittime = 0;
		isWait = false;
		CheckMenuItem(hmn, ID_AI32791, MFS_CHECKED);
		CheckMenuItem(hmn, ID_AI32792, MFS_UNCHECKED);
		init();
		sw->restart();
		searchcnt[0] = 0;
		searchcnt[1] = 0;

	}

	double df_search(int depth, int nowturn, int myturn, Field& nowf, bool isPassed = false) {

		searchcnt[myturn - 1]++;

		random_device rnd;     // 非決定的な乱数生成器を生成
		mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
		uniform_real_distribution<double> rand1(0, 1);


		if (depth == AIdepth[myturn - 1] && nowturn != myturn) {
			switch (evaltype[2 - nowturn]) {
			case EvalType::RANDOM:
				return rand1(mt);
				break;
			case EvalType::GREEDY:
				return greedyAI(nowf);
			default:
				return rand1(mt);
			//case AItype::GREEDYAI:
			//	return 0;
			//case AItype::EVALUATIONAI:
			//	return 0;
			//default:
			//	return 0;
			}
		}


		double maxresult = -INF;
		int nextdepth = depth;
		if (nowturn != myturn) {
			nextdepth++;
		}
		int nextturn = nowturn + 1;
		if (nextturn == 3) {
			nextturn = 1;
		}
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				pair<int, int> nextpnt = make_pair(x, y);
				Field nextfield = nowf;
				// f に nextpnt をおけるかチェックするおけたら
				if (nextfield.canPutOrNot(x, y, false, nowturn)) {

					nextfield.canPutOrNot(x, y, true, nowturn);

					double hyouka = df_search(nextdepth, nextturn, myturn, nextfield);

					if (nowturn != myturn) {
						hyouka = -hyouka;
					}
					if (hyouka > maxresult) {
						maxresult = hyouka;
						if (nowturn == myturn && depth == 1) {
							bestmove = nextpnt;
						}
					}
				}

			}
		}
		if (maxresult == -INF) {
			if (isPassed) {
				return (nowf.numOfEachPiece[turn] - nowf.numOfEachPiece[3 - turn]);
			}
			else {
				//changeTurn();
				return df_search(nextdepth, nextturn, myturn, nowf, true);
			}
		}

		if (nowturn != myturn) {
			maxresult = -maxresult;
		}
		return maxresult;
	}

	double ab_search(int depth, int maxdepth, int nowturn, int myturn, Field& nowf, bool isPassed, double alpha, double beta) {
		searchcnt[myturn - 1]++;

		random_device rnd;     // 非決定的な乱数生成器を生成
//		mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
		mt19937 mt(20191120);     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
		uniform_real_distribution<double> rand1(0, 1);

		if (depth == maxdepth && nowturn != myturn) {
			switch (evaltype[2 - nowturn]) {
			case EvalType::RANDOM:
				return rand1(mt);
				break;
			case EvalType::GREEDY:
				return greedyAI(nowf);
			case EvalType::PUTNUMPRIORITY:
				return canPutNum(nowf);
			default:
				return rand1(mt);
				//case AItype::GREEDYAI:
				//	return 0;
				//case AItype::EVALUATIONAI:
				//	return 0;
				//default:
				//	return 0;
			}
		}

		uint64_t hash = nowf.calc_hash();
		bool found;
		TT_DATA* ttdata = tt.findcache(hash, found);

		if (found) {
			return ttdata->data;
		}

		double maxresult = -INF;
		int nextdepth = depth;
		if (nowturn != myturn) {
			nextdepth++;
		}
		int nextturn = nowturn + 1;
		if (nextturn == 3) {
			nextturn = 1;
		}
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				pair<int, int> nextpnt = make_pair(x, y);
				Field nextfield = nowf;
				// f に nextpnt をおけるかチェックするおけたら
				if (nextfield.canPutOrNot(x, y, false, nowturn)) {

					nextfield.canPutOrNot(x, y, true, nowturn);

					double hyouka;

					//相手のターン
					if (nowturn != myturn) {
						hyouka = ab_search(nextdepth, maxdepth, nextturn, myturn, nextfield, false, alpha, -maxresult);
						if (hyouka < alpha) {
							ttdata->set(hash, alpha);
							return alpha;
						}
						hyouka = -hyouka;	
					}
					//自分のターン
					else {
						hyouka = ab_search(nextdepth, maxdepth, nextturn, myturn, nextfield, false, maxresult, beta);
						if (hyouka > beta) {
							ttdata->set(hash, beta);

							return beta;
						}
					}
					if (hyouka > maxresult) {
						maxresult = hyouka;
						if (nowturn == myturn && depth == 1) {
							bestmove = nextpnt;
						}
					}
				}

			}
		}
		if (maxresult == -INF) {
			if (isPassed) {
				double hyo = (nowf.numOfEachPiece[turn] - nowf.numOfEachPiece[3 - turn]) * 1000;
				ttdata->set(hash, hyo);
				tt.dump_count();
				return hyo;
			}
			else {
				//changeTurn();
				double hyo = ab_search(nextdepth, maxdepth, nextturn, myturn, nowf, true, alpha, beta);
				ttdata->set(hash, hyo);
				return hyo;
			}
		}

		if (nowturn != myturn) {
			maxresult = -maxresult;
			alpha = maxresult;
		}
		else {
			beta = maxresult;
		}
		ttdata->set(hash, maxresult);
		return maxresult;
	}

	class Fielddata {
	public:
		Field curfld;
		Fielddata* prevflddata;
		pair<int, int> movement;
		double eval = 0;
		bool isPassed = false;
		bool isFinished = false;

		Fielddata(const Field& f): curfld(f), prevflddata(nullptr), eval(0), movement(make_pair(0, 0)){}
		Fielddata(const Field& f, Fielddata* pf, const double ev, pair<int, int> mv) :curfld(f), prevflddata(pf), eval(ev), movement(mv){};
	};

	static bool greaterEval(const Fielddata& f, const Fielddata& s) {
		return f.eval > s.eval;
	}
	/*static bool lessEval(const Fielddata& f, const Fielddata& s) {
		return f.eval < s.eval;
	}*/

	void beamSearch(int d, int w, Field nowf, int nowturn, int myturn, EvalType etype) {
		vector<vector<Fielddata>> fields(d + 1);
		Fielddata firstfld(nowf);

		fields[0].push_back(firstfld);

		for (int i = 0; i < d; i++) {
			int cnt = 0;
			for (auto& f : fields[i]) {
				if (cnt >= w) {
					break;
				}
				cnt++;

				//if (f.movement == make_pair(-1, -1)) {
				//	Field finishedfld = f.curfld;
				//	Fielddata finisheddata(finishedfld, &f, f.eval, make_pair(-1, -1));
				//	fields[i].push_back(finisheddata);
				//	continue;
				//}

				bool put = false;

				if (f.isFinished) {
					Fielddata nextfld = f;
					nextfld.prevflddata = &f;
					nextfld.isFinished = true;
					fields[i + 1].push_back(nextfld);
					continue;
				}

				for (int y = 0; y < 8; y++) {
					for (int x = 0; x < 8; x++) {
						if (!f.curfld.canPutOrNot(x, y, false, nowturn)) {
							continue;
						}

						put = true;

						//double eval = f.curfld.calcOpenness(x, y, nowturn);

						Field tmpfld = f.curfld;
						
						f.curfld.canPutOrNot(x, y, true, nowturn);
						double eval;
						if (etype == EvalType::GREEDY) {
							eval = greedyAI(f.curfld);
						}
						else if (etype == EvalType::PUTNUMPRIORITY) {
							eval = canPutNum(f.curfld);
						}
						 
						Field nextf = f.curfld;
						

						Fielddata nextfld(nextf, &f, eval + f.eval, make_pair(x, y));

						nextfld.isPassed = false;

						fields[i + 1].push_back(nextfld);

						f.curfld = tmpfld;

					}
				}

				f.curfld.coutNumOfEachPieces();
				double ev;

				// この深さでおけなかった場合
				if (!put) {
					// 全部埋まってるから終わり
					if (f.curfld.numOfEachPiece[myturn] + f.curfld.numOfEachPiece[3 - myturn] == 64) {
						//if (f.curfld.numOfEachPiece[myturn] > f.curfld.numOfEachPiece[3 - myturn]) {
							ev = f.curfld.numOfEachPiece[myturn] - f.curfld.numOfEachPiece[3 - myturn] * FINISHEVAL;
							Field finishedfld = f.curfld;
							Fielddata finisheddata(finishedfld, &f, ev, make_pair(-1, -1));
							finisheddata.isFinished = true;
							fields[i + 1].push_back(finisheddata);
						//}

					} 
					// 前の敵がパスしたから、終わり
					else if (f.isPassed) {
						//if (f.curfld.numOfEachPiece[myturn] > f.curfld.numOfEachPiece[3 - myturn]) {
							ev = f.curfld.numOfEachPiece[myturn] - f.curfld.numOfEachPiece[3 - myturn]  * FINISHEVAL;
							Field finishedfld = f.curfld;
							Fielddata finisheddata(finishedfld, &f, ev, make_pair(-1, -1));
							finisheddata.isFinished = true;
							fields[i + 1].push_back(finisheddata);
						//}
					}
					// 前の敵はパスしてない。自分がパスする。同じ状況で次にまわす
					else {
						Field nextf = f.curfld;
						ev = f.eval;
						Fielddata nextfld(nextf, &f, ev, make_pair(f.movement.first, f.movement.second));
						nextfld.isPassed = true;
						fields[i + 1].push_back(nextfld);
					}
				}

			}


			if (nowturn == myturn) {
				sort(fields[i + 1].begin(), fields[i + 1].end(), greaterEval);
			}

			if (nowturn == 1) {
				nowturn = 2;
			}
			else {
				nowturn = 1;
			}
		}

		if (fields[d].size() == 0) {
			int a = 1;
		}
		Fielddata* fptr = &(fields[d][0]);
		Fielddata* bestfptr = nullptr;

		while (fptr) {
			if (fptr->prevflddata->prevflddata == nullptr) {
				bestfptr = fptr;
				break;
			}
			fptr = fptr->prevflddata;
		}

		if (bestfptr != nullptr) {
			bestmove = fptr->movement;
		}
	}

	void mixbeamSearch(int d, int w, Field nowf, int nowturn, int myturn, EvalType etype) {
		vector<vector<Fielddata>> fields(d + 1);
		Fielddata firstfld(nowf);

		fields[0].push_back(firstfld);

		for (int i = 0; i < d; i++) {
			int cnt = 0;
			for (auto& f : fields[i]) {
				if (cnt >= w) {
					break;
				}
				cnt++;

				//if (f.movement == make_pair(-1, -1)) {
				//	Field finishedfld = f.curfld;
				//	Fielddata finisheddata(finishedfld, &f, f.eval, make_pair(-1, -1));
				//	fields[i].push_back(finisheddata);
				//	continue;
				//}

				bool put = false;

				if (f.isFinished) {
					Fielddata nextfld = f;
					nextfld.prevflddata = &f;
					nextfld.isFinished = true;
					fields[i + 1].push_back(nextfld);
					continue;
				}

				for (int y = 0; y < 8; y++) {
					for (int x = 0; x < 8; x++) {
						if (!f.curfld.canPutOrNot(x, y, false, nowturn)) {
							continue;
						}

						put = true;

						//double eval = f.curfld.calcOpenness(x, y, nowturn);

						Field tmpfld = f.curfld;
						double eval;

						random_device rnd;     // 非決定的な乱数生成器を生成
						mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
						uniform_real_distribution<double> rand1(0, 1);

						eval = f.curfld.calcOpenness(x, y, nowturn) + rand1(mt);

						f.curfld.canPutOrNot(x, y, true, nowturn);

						Field nextf = f.curfld;

						//if (nowturn == myturn) {
						//	eval = f.eval - eval;
						//}
						//else {
							eval = f.eval + eval;
						//}

						Fielddata nextfld(nextf, &f, eval, make_pair(x, y));

						nextfld.isPassed = false;

						fields[i + 1].push_back(nextfld);

						f.curfld = tmpfld;

					}
				}

				f.curfld.coutNumOfEachPieces();
				double ev;

				// この深さでおけなかった場合
				if (!put) {
					// 全部埋まってるから終わり
					if (f.curfld.numOfEachPiece[myturn] + f.curfld.numOfEachPiece[3 - myturn] == 64) {
						//if (f.curfld.numOfEachPiece[myturn] > f.curfld.numOfEachPiece[3 - myturn]) {
						ev = f.curfld.numOfEachPiece[myturn] - f.curfld.numOfEachPiece[3 - myturn] * FINISHEVAL;
						Field finishedfld = f.curfld;
						Fielddata finisheddata(finishedfld, &f, ev, make_pair(-1, -1));
						finisheddata.isFinished = true;
						fields[i + 1].push_back(finisheddata);
						//}

					}
					// 前の敵がパスしたから、終わり
					else if (f.isPassed) {
						//if (f.curfld.numOfEachPiece[myturn] > f.curfld.numOfEachPiece[3 - myturn]) {
						ev = f.curfld.numOfEachPiece[myturn] - f.curfld.numOfEachPiece[3 - myturn] * FINISHEVAL;
						Field finishedfld = f.curfld;
						Fielddata finisheddata(finishedfld, &f, ev, make_pair(-1, -1));
						finisheddata.isFinished = true;
						fields[i + 1].push_back(finisheddata);
						//}
					}
					// 前の敵はパスしてない。自分がパスする。同じ状況で次にまわす
					else {
						Field nextf = f.curfld;
						ev = f.eval;
						Fielddata nextfld(nextf, &f, ev, make_pair(f.movement.first, f.movement.second));
						nextfld.isPassed = true;
						fields[i + 1].push_back(nextfld);
					}
				}

			}


			if (nowturn == myturn) {
				sort(fields[i + 1].begin(), fields[i + 1].end(), greaterEval);
			}

			if (nowturn == 1) {
				nowturn = 2;
			}
			else {
				nowturn = 1;
			}
		}

		if (fields[d].size() == 0) {
			int a = 1;
		}
		Fielddata* fptr = &(fields[d][0]);
		Fielddata* bestfptr = nullptr;

		while (fptr) {
			if (fptr->prevflddata->prevflddata == nullptr) {
				bestfptr = fptr;
				break;
			}
			fptr = fptr->prevflddata;
		}

		if (bestfptr != nullptr) {
			bestmove = fptr->movement;
		}
	}


	void mixAI(int beamdepth, int beamwidth, int dfsstart) {

		if (emptyNum <= 15) {
			ab_search(1, emptyNum, turn, turn, f, false, -INF, INF);
			return;
		}
		else {
			mixbeamSearch(AIdepth[turn - 1], AIwidth[turn - 1], f, turn, turn, evaltype[turn - 1]);
			return;
		}

	}
};
