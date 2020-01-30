//#pragma once
#ifndef BOARD_H
#define BOARD_H

#include "ChessType.h"

namespace BoardSpace {

class Board {
public:
    Board(const wstring& pieceChars = wstring{});

    bool isBottomSide(PieceColor color) const { return bottomColor_ == color; }
    bool isKilled(PieceColor color) const;
    bool isDied(PieceColor color) const;

    SSeat_pair getSeatPair(int frow, int fcol, int trow, int tcol) const;
    SSeat_pair getSeatPair(int frowcol, int trowcol) const;
    SSeat_pair getSeatPair(const wstring& str, RecFormat fmt) const;

    template <typename From_T1, typename From_T2>
    const vector<pair<int, int>> getCanMoveRowCols(From_T1 arg1, From_T2 arg2) const;
    //SSeat_vector getCanMoveSeats(const wstring& str, RecFormat fmt) const;

    const vector<pair<int, int>> getLiveRowCols(PieceColor color) const;

    void setPieces(const wstring& pieceChars);
    void changeSide(const ChangeType ct);

    const wstring getZhStr(SSeat_pair seat_pair) const;
    const wstring getPieceChars() const;
    const wstring toString() const;

private:
    PieceColor bottomColor_;
    shared_ptr<Pieces> pieces_;
    shared_ptr<Seats> seats_;

    void __setBottomSide();

    const SSeat& __getSeat(int row, int col) const;
    const SSeat& __getSeat(const wstring& str, RecFormat fmt) const;
    const SSeat& __getSeatFromZhStr(const wstring& zhStr) const;
    SSeat_pair __getSeatPairFromZhStr(const wstring& zhStr) const;

    SSeat_vector __getCanMoveSeats(const SSeat& fseat) const;
};
}

#endif