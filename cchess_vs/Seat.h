//#pragma once
#ifndef SEAT_H
#define SEAT_H

#include "ChessType.h"

namespace SeatSpace {

// ����λ����
class Seat : public enable_shared_from_this<Seat> {

public:
    explicit Seat(int row, int col);

    int row() const { return row_; }
    int col() const { return col_; }
    int rowcol() const { return row_ * 10 + col_; }
    const SPiece& piece() const { return piece_; }

    bool isSameColor(const SSeat& seat) const;
    void setPiece(const SPiece& piece = nullptr) { piece_ = piece; }
    const SPiece& movTo(SSeat& tseat, const SPiece& eatPiece = nullptr);

    const wstring toString() const;

private:
    const int row_, col_;
    SPiece piece_{};
};

// ����λ����
class Seats {
public:
    Seats();

    const SSeat& getSeat(int row, int col) const;
    const SSeat& getSeat(int rowcol) const;
    const SSeat& getSeat(const pair<int, int>& rowcol_pair) const
    {
        return getSeat(rowcol_pair.first, rowcol_pair.second);
    }
    const SSeat& getKingSeat(bool isBottom) const;

    SSeat_pair getSeatPair(int frow, int fcol, int trow, int tcol) const
    {
        return make_pair(getSeat(frow, fcol), getSeat(trow, tcol));
    }
    SSeat_pair getSeatPair(int frowcol, int trowcol) const
    {
        return make_pair(getSeat(frowcol), getSeat(trowcol));
    }

    // ���ӿɷ��õ�λ��
    SSeat_vector putSeats(bool isBottom, const SPiece& piece) const;
    // ĳλ�����ӿ��ƶ���λ�ã�δ�ų��������������
    SSeat_vector getMoveSeats(bool isBottom, const SSeat& fseat) const;
    // ȡ�������ϻ������
    SSeat_vector getLiveSeats(PieceColor color, wchar_t name = BLANKNAME,
        int col = BLANKCOL, bool getStronge = false) const;
    // '�������'
    SSeat_vector getSortPawnLiveSeats(bool isBottom, PieceColor color, wchar_t name) const;

    void setPieces(const vector<SPiece>& boardPieces);
    void changeSide(const ChangeType ct, const shared_ptr<PieceSpace::Pieces>& pieces);
    const wstring getPieceChars() const;
    const wstring toString() const;

private:
    SSeat_vector allSeats_{};

    SSeat_vector getAllSeats() const;
    SSeat_vector getKingSeats(bool isBottom) const;
    SSeat_vector getAdvisorSeats(bool isBottom) const;
    SSeat_vector getBishopSeats(bool isBottom) const;
    SSeat_vector getPawnSeats(bool isBottom) const;

    SSeat_vector getKingMoveSeats(bool isBottom, const SSeat& fseat) const;
    SSeat_vector getAdvisorMoveSeats(bool isBottom, const SSeat& fseat) const;
    SSeat_vector getBishopMoveSeats(bool isBottom, const SSeat& fseat) const;
    SSeat_vector getKnightMoveSeats(bool isBottom, const SSeat& fseat) const;
    SSeat_vector getRookMoveSeats(const SSeat& fseat) const;
    SSeat_vector getCannonMoveSeats(const SSeat& fseat) const;
    SSeat_vector getPawnMoveSeats(bool isBottom, const SSeat& fseat) const;

    SSeat_vector __getSeats(const vector<pair<int, int>>& rowcols) const;
    // �ų�ͬ��ɫ����
    SSeat_vector __getMoveSeats(const vector<pair<int, int>>& rowcols, const SSeat& fseat) const;

    const vector<pair<int, int>> __getNonObs_MoveRowcols(bool isBottom, const SSeat& fseat,
        const vector<pair<pair<int, int>, pair<int, int>>> getObs_MoveRowcols(bool, int, int)) const;
    const vector<pair<int, int>> __getRook_MoveRowcols(const SSeat& fseat) const;
    const vector<pair<int, int>> __getCannon_MoveRowcols(const SSeat& fseat) const;
};

// ����λ�ù�����
class SeatManager {
public:
    static bool isBottom(int row) { return row < RowLowUpIndex_; };
    static int getIndex_rc(int row, int col) { return row * BOARDCOLNUM + col; }
    static int getIndex_rc(int rowcol) { return getIndex_rc(rowcol / 10, rowcol % 10); }
    static int getRotate(int rowcol) { return (BOARDROWNUM - rowcol / 10 - 1) * 10 + (BOARDCOLNUM - rowcol % 10 - 1); }
    static int getSymmetry(int rowcol) { return rowcol + BOARDCOLNUM - rowcol % 10 * 2 - 1; }

    static void movBack(SSeat& fseat, SSeat& tseat, const SPiece& eatPiece);

    static const vector<pair<int, int>> getRowCols(const SSeat_vector& seats);

    static const vector<pair<int, int>> getAllRowcols();
    static const vector<pair<int, int>> getKingRowcols(bool isBottom);
    static const vector<pair<int, int>> getAdvisorRowcols(bool isBottom);
    static const vector<pair<int, int>> getBishopRowcols(bool isBottom);
    static const vector<pair<int, int>> getPawnRowcols(bool isBottom);

    static const vector<pair<int, int>> getKingMoveRowcols(bool isBottom, int frow, int fcol);
    static const vector<pair<int, int>> getAdvisorMoveRowcols(bool isBottom, int frow, int fcol);
    static const vector<pair<pair<int, int>, pair<int, int>>> getBishopObs_MoveRowcols(bool isBottom, int frow, int fcol);
    static const vector<pair<pair<int, int>, pair<int, int>>> getKnightObs_MoveRowcols(bool isBottom, int frow, int fcol);
    static const vector<vector<pair<int, int>>> getRookCannonMoveRowcol_Lines(int frow, int fcol);
    static const vector<pair<int, int>> getPawnMoveRowcols(bool isBottom, int frow, int fcol);

private:
    static constexpr int RowLowIndex_{ 0 }, RowLowMidIndex_{ 2 }, RowLowUpIndex_{ 4 },
        RowUpLowIndex_{ 5 }, RowUpMidIndex_{ 7 }, RowUpIndex_{ 9 },
        ColLowIndex_{ 0 }, ColMidLowIndex_{ 3 }, ColMidUpIndex_{ 5 }, ColUpIndex_{ 8 };
};

const wstring getSeatsStr(const SSeat_vector& seats);
const wstring getRowColsStr(const vector<pair<int, int>>& rowcols);
}

#endif
