//#pragma once
#ifndef SEAT_H
#define SEAT_H

//#include <map>
#include <memory>
#include <string>
#include <vector>

//*
using namespace std;

enum class PieceColor;
enum class ChangeType {
    EXCHANGE,
    ROTATE,
    SYMMETRY
};

namespace BoardSpace {
class Board;
}
//*/

namespace PieceSpace {
class Piece;
class Pieces;
}

namespace SeatSpace {
class Seats;

class Seat : public enable_shared_from_this<Seat> {
public:
    explicit Seat(int row, int col);

    const int row() const { return row_; }
    const int col() const { return col_; }
    const int rowcol() const { return row_ * 10 + col_; }
    const shared_ptr<PieceSpace::Piece>& piece() const { return piece_; }

    void put(const shared_ptr<PieceSpace::Piece>& piece = nullptr);
    const vector<shared_ptr<Seat>> getMoveSeats(bool isBottom, const shared_ptr<Seats>& seats) const;

    const wstring toString() const;

private:
    const int row_, col_;
    shared_ptr<PieceSpace::Piece> piece_ {};
};

class Seats {
    friend class BoardSpace::Board;

public:
    const shared_ptr<Seat>& getSeat(const int row, const int col) const;
    const shared_ptr<Seat>& getSeat(const int rowcol) const;
    const shared_ptr<Seat>& getSeat(const pair<int, int>& rowcol) const
    {
        return getSeat(rowcol.first, rowcol.second);
    }

    const vector<shared_ptr<Seat>>
    getLiveSeats(const PieceColor color, const wchar_t name = L'\x0',
        const int col = -1, bool getStronge = false) const;
    // '¶à±øÅÅÐò'
    const vector<shared_ptr<Seat>>
    getSortPawnLiveSeats(bool isBottom, const PieceColor color, const wchar_t name) const;

    void reset(const vector<shared_ptr<PieceSpace::Piece>>& boardPieces);
    void changeSide(const ChangeType ct, const shared_ptr<PieceSpace::Pieces>& pieces);

    const wstring getPieceChars() const;
    const wstring toString() const;

    const vector<shared_ptr<Seat>> getAllSeats() const;
    const vector<shared_ptr<Seat>> getKingSeats(bool isBottom) const;
    const vector<shared_ptr<Seat>> getAdvisorSeats(bool isBottom) const;
    const vector<shared_ptr<Seat>> getBishopSeats(bool isBottom) const;
    const vector<shared_ptr<Seat>> getPawnSeats(bool isBottom) const;

    const vector<shared_ptr<Seat>> getKingMoveSeats(bool isBottom, const Seat& fseat) const;
    const vector<shared_ptr<Seat>> getAdvisorMoveSeats(bool isBottom, const Seat& fseat) const;
    const vector<shared_ptr<Seat>> getBishopMoveSeats(bool isBottom, const Seat& fseat) const;
    const vector<shared_ptr<Seat>> getKnightMoveSeats(const Seat& fseat) const;
    const vector<shared_ptr<Seat>> getRookMoveSeats(const Seat& fseat) const;
    const vector<shared_ptr<Seat>> getCannonMoveSeats(const Seat& fseat) const;
    const vector<shared_ptr<Seat>> getPawnMoveSeats(bool isBottom, const Seat& fseat) const;

    const wstring getSeatsStr(const vector<shared_ptr<Seat>>& seats) const;

    static const int ColNum() { return ColNum_; };
    static const bool isBottom(const int row) { return row < RowLowUpIndex_; };
    static const int getIndex(const int row, const int col) { return row * ColNum_ + col; }
    static const int getIndex(const int rowcol) { return rowcol / 10 * ColNum_ + rowcol % 10; }
    static const int getRotate(int rowcol) { return (RowNum_ - rowcol / 10 - 1) * 10 + (ColNum_ - rowcol % 10 - 1); }
    static const int getSymmetry(int rowcol) { return rowcol + ColNum_ - rowcol % 10 * 2 - 1; }

private:
    Seats();

    vector<shared_ptr<Seat>> allSeats_;

    const vector<shared_ptr<Seat>>
    __getSeats(vector<pair<int, int>> rowcols) const;
    const vector<pair<shared_ptr<Seat>, shared_ptr<Seat>>>
    __getBishopObs_MoveSeats(bool isBottom, const Seat& fseat) const;
    const vector<pair<shared_ptr<Seat>, shared_ptr<Seat>>>
    __getKnightObs_MoveSeats(const Seat& fseat) const;
    const vector<shared_ptr<Seat>>
    __getNonObstacleSeats(const vector<pair<shared_ptr<Seat>,
            shared_ptr<Seat>>>& obs_MoveSeats) const;
    const vector<vector<shared_ptr<Seat>>>
    __getRookCannonMoveSeat_Lines(const Seat& fseat) const;

    static const int RowNum_ { 10 }, ColNum_ { 9 },
        RowLowIndex_ { 0 }, RowLowMidIndex_ { 2 }, RowLowUpIndex_ { 4 },
        RowUpLowIndex_ { 5 }, RowUpMidIndex_ { 7 }, RowUpIndex_ { 9 },
        ColLowIndex_ { 0 }, ColMidLowIndex_ { 3 }, ColMidUpIndex_ { 5 }, ColUpIndex_ { 8 };
};

}

#endif
