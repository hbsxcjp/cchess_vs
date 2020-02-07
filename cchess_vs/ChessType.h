//#pragma once
#ifndef CHESSTYPE_H
#define CHESSTYPE_H

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <direct.h>
#include <fstream>
#include <functional>
#include <io.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace PieceSpace {
class Piece;
class Pieces;
class PieceManager;
}

namespace SeatSpace {
class Seat;
class Seats;
class SeatManager;
}

namespace BoardSpace {
class Board;
}

namespace ChessManualSpace {
class ChessManual;
}

using namespace std;
using namespace PieceSpace;
using namespace SeatSpace;
using namespace BoardSpace;
using namespace ChessManualSpace;

enum class PieceColor {
    RED,
    BLACK
};

enum class PieceKind {
    KING,
    ADVISOR,
    BISHOP,
    KNIGHT,
    ROOK,
    CANNON,
    PAWN
};

enum class ChangeType {
    EXCHANGE,
    ROTATE,
    SYMMETRY
};

enum class RecFormat {
    XQF,
    BIN,
    JSON,
    PGN_ICCS,
    PGN_ZH,
    PGN_CC
};

typedef shared_ptr<PieceSpace::Piece> SPiece;
typedef shared_ptr<SeatSpace::Seat> SSeat;
typedef pair<SSeat, SSeat> SSeat_pair; // 可以使用常量引用?
typedef vector<SSeat> SSeat_vector; // 容器装的是对象拷贝

typedef shared_ptr<BoardSpace::Board> SBoard;
typedef shared_ptr<ChessManualSpace::ChessManual> SChessManual;

constexpr auto BLANKNAME = L'\x0';
constexpr auto BLANKCOL = -1;
constexpr auto PIECENUM = 32;
constexpr auto BOARDROWNUM = 10;
constexpr auto BOARDCOLNUM = 9;
constexpr auto SEATNUM = BOARDROWNUM * BOARDCOLNUM;

#endif
