//#pragma once
#ifndef PIECE_H
#define PIECE_H

#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace SeatSpace {
class Seat;
class Seats;
}

namespace BoardSpace {
class Board;
}

enum class PieceColor {
    RED,
    BLACK
};

namespace PieceSpace {

// 棋子类
class Piece {
public:
    explicit Piece(const wchar_t ch);
    const wchar_t ch() const { return ch_; }
    const wchar_t name() const;
    const PieceColor color() const;

    void setSeat(const shared_ptr<SeatSpace::Seat>& seat = nullptr) { seat_ = seat; }

    const vector<shared_ptr<SeatSpace::Seat>> putSeats(bool isBottom, const SeatSpace::Seats& seats) const;

    const wstring toString() const;

private:
    const wchar_t ch_;
    shared_ptr<SeatSpace::Seat> seat_ {};
};

class Pieces {
    friend class BoardSpace::Board;

public:
    const shared_ptr<Piece>&
    getOtherPiece(const shared_ptr<Piece>& piece) const;

    const vector<shared_ptr<Piece>>
    getBoardPieces(const wstring& pieceChars) const;

    const wstring toString() const;

    static const wstring getZhChars()
    {
        return (preChars_ + nameChars_ + movChars_
            + numChars_.at(PieceColor::RED) + numChars_.at(PieceColor::BLACK));
    }

    static const wstring getICCSChars()
    {
        return to_wstring(1234567890) + ICCSChars_;
    }

    static const wstring getFENStr() { return FENStr_; }

    static const int getRowFromICCSChar(const wchar_t ch) { return ch - '0'; } // 0:48
    static const int getColFromICCSChar(const wchar_t ch) { return ICCSChars_.find(ch); }
    static const wchar_t getColICCSChar(const int col) { return ICCSChars_.at(col); }

    static const wchar_t getName(const wchar_t ch);
    static const wchar_t getPrintName(const Piece& piece);
    static const PieceColor getColor(const wchar_t ch);
    static const PieceColor getColorFromZh(const wchar_t numZh);
    static const int getIndex(const int seatsLen, const bool isBottom, const wchar_t preChar);
    static const wchar_t getIndexChar(const int seatsLen, const bool isBottom, const int index);

    static const wchar_t nullChar() { return nullChar_; };

    static const int getMovNum(const bool isBottom, const wchar_t movChar)
    {
        return (static_cast<int>(movChars_.find(movChar)) - 1) * (isBottom ? 1 : -1);
    }

    static const wchar_t getMovChar(const bool isSameRow, bool isBottom, bool isLowToUp)
    {
        return movChars_.at(isSameRow ? 1 : (isBottom == isLowToUp ? 2 : 0));
    }

    static const int getNum(const PieceColor color, const wchar_t numChar)
    {
        return static_cast<int>(numChars_.at(color).find(numChar)) + 1;
    }

    static const wchar_t getNumChar(const PieceColor color, const int num)
    {
        return numChars_.at(color).at(num - 1);
    }

    static const int getCol(bool isBottom, const int num);
    static const wchar_t getColChar(const PieceColor color, bool isBottom, const int col);

    static const bool isKing(const wchar_t name)
    {
        return nameChars_.substr(0, 2).find(name) != wstring::npos;
    }

    static const bool isAdvisor(const wchar_t name)
    {
        return nameChars_.substr(2, 2).find(name) != wstring::npos;
    }

    static const bool isBishop(const wchar_t name)
    {
        return nameChars_.substr(4, 2).find(name) != wstring::npos;
    }

    static const bool isKnight(const wchar_t name)
    {
        return nameChars_.at(6) == name;
    }

    static const bool isRook(const wchar_t name)
    {
        return nameChars_.at(7) == name;
    }

    static const bool isCannon(const wchar_t name)
    {
        return nameChars_.at(8) == name;
    }

    static const bool isStronge(const wchar_t name)
    {
        return nameChars_.substr(6, 5).find(name) != wstring::npos;
    }

    static const bool isLineMove(const wchar_t name)
    {
        return isKing(name) || nameChars_.substr(7, 4).find(name) != wstring::npos;
    }

    static const bool isPawn(const wchar_t name)
    {
        return nameChars_.substr(nameChars_.size() - 2, 2).find(name) != wstring::npos;
    }

    static const bool isPiece(const wchar_t name)
    {
        return nameChars_.find(name) != wstring::npos;
    };

private:
    Pieces();

    vector<shared_ptr<Piece>> allPieces_;

    static const wstring __getPreChars(const int length)
    {
        return (length == 2 ? (wstring { preChars_ }).erase(1, 1) //L"前后"
                            : (length == 3 ? preChars_
                                           //L"一二三四五");
                                           : numChars_.at(PieceColor::RED).substr(0, 5)));
    }

    static const wstring chChars_;
    static const wstring preChars_;
    static const wstring nameChars_;
    static const wstring movChars_;
    static const map<PieceColor, wstring> numChars_;
    static const wchar_t nullChar_;
    static const wstring ICCSChars_;
    static const wstring FENStr_;
};

}

#endif