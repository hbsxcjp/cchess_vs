#include "Seat.h"
#include "Piece.h"
#include <algorithm>
#include <cassert>
#include <sstream>

using namespace std;
using namespace PieceSpace;

namespace SeatSpace {

Seat::Seat(int row, int col)
    : row_ { row }
    , col_ { col }
{
}

void Seat::put(const shared_ptr<Piece>& piece)
{
    if (piece_ = piece)
        piece->setSeat(shared_from_this());
}

const vector<shared_ptr<Seat>> Seat::getMoveSeats(bool isBottom, const shared_ptr<Seats>& seats) const
{
    auto name = piece_->name();
    if (Pieces::isRook(name))
        return seats->getRookMoveSeats(*this);
    else if (Pieces::isKnight(name))
        return seats->getKnightMoveSeats(*this);
    else if (Pieces::isCannon(name))
        return seats->getCannonMoveSeats(*this);
    else if (Pieces::isPawn(name))
        return seats->getPawnMoveSeats(isBottom, *this);
    else if (Pieces::isBishop(name))
        return seats->getBishopMoveSeats(isBottom, *this);
    else if (Pieces::isAdvisor(name))
        return seats->getAdvisorMoveSeats(isBottom, *this);
    else // isKing
        return seats->getKingMoveSeats(isBottom, *this);
}

const wstring Seat::toString() const
{
    wstringstream wss {};
    wss << row_ << col_ << (piece_ ? piece_->name() : L'_'); //<< boolalpha << setw(2) <<
    return wss.str();
}

Seats::Seats()
{
    for (int row = 0; row < RowNum_; ++row)
        for (int col = 0; col < ColNum_; ++col)
            allSeats_.push_back(make_shared<Seat>(row, col));
}

const shared_ptr<Seat>& Seats::getSeat(const int row, const int col) const
{
    return allSeats_.at(row * ColNum_ + col);
}

const shared_ptr<Seat>& Seats::getSeat(const int rowcol) const
{
    return allSeats_.at(rowcol / 10 * ColNum_ + rowcol % 10);
}

const vector<shared_ptr<Seat>>
Seats::getLiveSeats(const PieceColor color, const wchar_t name, const int col, bool getStronge) const
{
    vector<shared_ptr<Seat>> seats {};
    copy_if(allSeats_.begin(), allSeats_.end(), back_inserter(seats),
        [&](const shared_ptr<Seat>& seat) {
            auto piece = seat->piece();
            return piece && color == piece->color()
                && (name == L'\x0' || name == piece->name())
                && (col == -1 || col == seat->col())
                && (!getStronge || Pieces::isStronge(piece->name()));
        });
    return seats;
}

const vector<shared_ptr<Seat>>
Seats::getSortPawnLiveSeats(bool isBottom, const PieceColor color, const wchar_t name) const
{
    // 最多5个兵
    vector<shared_ptr<Seat>> pawnSeats { getLiveSeats(color, name) }, seats {};
    // 按列建立字典，按列排序
    map<int, vector<shared_ptr<Seat>>> colSeats {};
    for_each(pawnSeats.begin(), pawnSeats.end(),
        [&](const shared_ptr<Seat>& seat) {
            colSeats[isBottom ? -seat->col() : seat->col()].push_back(seat);
        }); // 底边则列倒序,每列位置倒序

    // 整合成一个数组
    for_each(colSeats.begin(), colSeats.end(),
        [&](const pair<int, vector<shared_ptr<Seat>>>& colSeat) {
            if (colSeat.second.size() > 1) // 筛除只有一个位置的列
                copy(colSeat.second.begin(), colSeat.second.end(), back_inserter(seats));
        }); //按列存入
    return seats;
}

void Seats::reset(const vector<shared_ptr<Piece>>& boardPieces)
{
    assert(allSeats_.size() == boardPieces.size());
    int index { 0 };
    for_each(allSeats_.begin(), allSeats_.end(),
        [&](const shared_ptr<Seat>& seat) { seat->put(boardPieces[index++]); });
}

void Seats::changeSide(const ChangeType ct, const shared_ptr<Pieces>& pieces)
{
    vector<shared_ptr<Piece>> boardPieces {};
    auto changeRowcol = (ct == ChangeType::ROTATE ? &getRotate : &getSymmetry);
    for_each(allSeats_.begin(), allSeats_.end(),
        [&](const shared_ptr<Seat>& seat) {
            boardPieces.push_back(ct == ChangeType::EXCHANGE
                    ? pieces->getOtherPiece(seat->piece())
                    : getSeat(changeRowcol(seat->rowcol()))->piece());
        });
    reset(boardPieces);
}

const wstring Seats::getPieceChars() const
{
    wstringstream wss {};
    for_each(allSeats_.begin(), allSeats_.end(),
        [&](const shared_ptr<Seat>& seat) {
            wss << (seat->piece() ? seat->piece()->ch() : Pieces::nullChar());
        });
    return wss.str();
}

const wstring Seats::toString() const
{
    wstringstream wss {};
    for_each(allSeats_.begin(), allSeats_.end(),
        [&](const shared_ptr<Seat>& seat) {
            wss << seat->toString() << L' ';
        });
    return wss.str();
}

const std::vector<std::shared_ptr<Seat>> Seats::getAllSeats() const
{
    return allSeats_;
}

const vector<shared_ptr<Seat>> Seats::getKingSeats(bool isBottom) const
{
    vector<shared_ptr<Seat>> seats {};
    int rowLow { isBottom ? RowLowIndex_ : RowUpMidIndex_ },
        rowUp { isBottom ? RowLowMidIndex_ : RowUpIndex_ };
    for (int row = rowLow; row <= rowUp; ++row)
        for (int col = ColMidLowIndex_; col <= ColMidUpIndex_; ++col)
            seats.push_back(getSeat(row, col));
    return seats;
}

const vector<shared_ptr<Seat>> Seats::getAdvisorSeats(bool isBottom) const
{
    vector<shared_ptr<Seat>> seats {};
    int rowLow { isBottom ? RowLowIndex_ : RowUpMidIndex_ },
        rowUp { isBottom ? RowLowMidIndex_ : RowUpIndex_ }, rmd { isBottom ? 1 : 0 }; // 行列和的奇偶值
    for (int row = rowLow; row <= rowUp; ++row)
        for (int col = ColMidLowIndex_; col <= ColMidUpIndex_; ++col)
            if ((col + row) % 2 == rmd)
                seats.push_back(getSeat(row, col));
    return seats;
}

const vector<shared_ptr<Seat>> Seats::getBishopSeats(bool isBottom) const
{
    vector<shared_ptr<Seat>> seats {};
    int rowLow { isBottom ? RowLowIndex_ : RowUpLowIndex_ },
        rowUp { isBottom ? RowLowUpIndex_ : RowUpIndex_ };
    for (int row = rowLow; row <= rowUp; row += 2)
        for (int col = ColLowIndex_; col <= ColUpIndex_; col += 2) {
            int gap { row - col };
            if ((isBottom && (gap == 2 || gap == -2 || gap == -6))
                || (!isBottom && (gap == 7 || gap == 3 || gap == -1)))
                seats.push_back(getSeat(row, col));
        }
    return seats;
}

const vector<shared_ptr<Seat>> Seats::getPawnSeats(bool isBottom) const
{
    vector<shared_ptr<Seat>> seats {};
    int lfrow { isBottom ? RowLowUpIndex_ - 1 : RowUpLowIndex_ },
        ufrow { isBottom ? RowLowUpIndex_ : RowUpLowIndex_ + 1 },
        ltrow { isBottom ? RowUpLowIndex_ : RowLowIndex_ },
        utrow { isBottom ? RowUpIndex_ : RowLowUpIndex_ };
    for (int row = lfrow; row <= ufrow; ++row)
        for (int col = ColLowIndex_; col <= ColUpIndex_; col += 2)
            seats.push_back(getSeat(row, col));
    for (int row = ltrow; row <= utrow; ++row)
        for (int col = ColLowIndex_; col <= ColUpIndex_; ++col)
            seats.push_back(getSeat(row, col));
    return seats;
}

const std::vector<std::shared_ptr<Seat>> Seats::getKingMoveSeats(bool isBottom, const Seat& fseat) const
{
    int frow { fseat.row() }, fcol { fseat.col() };
    std::vector<std::pair<int, int>> rowcols {
        { frow, fcol - 1 }, { frow, fcol + 1 },
        { frow - 1, fcol }, { frow + 1, fcol }
    };
    int rowLow { isBottom ? RowLowIndex_ : RowUpMidIndex_ },
        rowUp { isBottom ? RowLowMidIndex_ : RowUpIndex_ };
    auto pos = std::remove_if(rowcols.begin(), rowcols.end(),
        [&](const std::pair<int, int>& rowcol) {
            return !(rowcol.first >= rowLow && rowcol.first <= rowUp
                && rowcol.second >= ColMidLowIndex_ && rowcol.second <= ColMidUpIndex_);
        });
    return __getSeats(std::vector<std::pair<int, int>> { rowcols.begin(), pos });
}

const std::vector<std::shared_ptr<Seat>> Seats::getAdvisorMoveSeats(bool isBottom, const Seat& fseat) const
{
    int frow { fseat.row() }, fcol { fseat.col() };
    std::vector<std::pair<int, int>> rowcols {
        { frow - 1, fcol - 1 }, { frow - 1, fcol + 1 },
        { frow + 1, fcol - 1 }, { frow + 1, fcol + 1 }
    };
    int rowLow { isBottom ? RowLowIndex_ : RowUpMidIndex_ },
        rowUp { isBottom ? RowLowMidIndex_ : RowUpIndex_ };
    auto pos = std::remove_if(rowcols.begin(), rowcols.end(),
        [&](const std::pair<int, int>& rowcol) {
            return !(rowcol.first >= rowLow && rowcol.first <= rowUp
                && rowcol.second >= ColMidLowIndex_ && rowcol.second <= ColMidUpIndex_);
        });
    return __getSeats(std::vector<std::pair<int, int>> { rowcols.begin(), pos });
}

const std::vector<std::shared_ptr<Seat>> Seats::getBishopMoveSeats(bool isBottom, const Seat& fseat) const
{
    return __getNonObstacleSeats(__getBishopObs_MoveSeats(isBottom, fseat));
}

const std::vector<std::shared_ptr<Seat>> Seats::getKnightMoveSeats(const Seat& fseat) const
{
    return __getNonObstacleSeats(__getKnightObs_MoveSeats(fseat));
}

const std::vector<std::shared_ptr<Seat>> Seats::getRookMoveSeats(const Seat& fseat) const
{
    std::vector<std::shared_ptr<Seat>> moveSeats {};
    for (auto& seats : __getRookCannonMoveSeat_Lines(fseat))
        for (auto& seat : seats) {
            moveSeats.push_back(seat);
            if (seat->piece())
                break;
        }
    return moveSeats;
}

const std::vector<std::shared_ptr<Seat>> Seats::getCannonMoveSeats(const Seat& fseat) const
{
    std::vector<std::shared_ptr<Seat>> moveSeats {};
    for (auto& seats : __getRookCannonMoveSeat_Lines(fseat)) {
        bool skip = false;
        for (auto& seat : seats) {
            if (!skip) {
                if (!seat->piece())
                    moveSeats.push_back(seat);
                else
                    skip = true;
            } else if (seat->piece()) {
                moveSeats.push_back(seat);
                break;
            }
        }
    }
    return moveSeats;
}

const std::vector<std::shared_ptr<Seat>> Seats::getPawnMoveSeats(bool isBottom, const Seat& fseat) const
{
    int frow { fseat.row() }, fcol { fseat.col() };
    std::vector<std::shared_ptr<Seat>> moveSeats {};
    int row {}, col {};
    if ((isBottom && (row = frow + 1) <= RowUpIndex_)
        || (!isBottom && (row = frow - 1) >= RowLowIndex_))
        moveSeats.push_back(getSeat(row, fcol));
    if (isBottom == (frow > RowLowUpIndex_)) { // 兵已过河
        if ((col = fcol - 1) >= ColLowIndex_)
            moveSeats.push_back(getSeat(frow, col));
        if ((col = fcol + 1) <= ColUpIndex_)
            moveSeats.push_back(getSeat(frow, col));
    }
    return moveSeats;
}

const std::wstring Seats::getSeatsStr(const std::vector<std::shared_ptr<Seat>>& seats) const
{
    std::wstringstream wss {};
    wss << seats.size() << L"个: ";
    std::for_each(seats.begin(), seats.end(),
        [&](const std::shared_ptr<Seat>& seat) {
            wss << seat->toString() << L' ';
        });
    return wss.str();
}

const std::vector<std::shared_ptr<Seat>> Seats::__getSeats(std::vector<std::pair<int, int>> rowcols) const
{
    std::vector<std::shared_ptr<Seat>> seats {};
    std::for_each(rowcols.begin(), rowcols.end(), [&](std::pair<int, int>& rowcol) {
        seats.push_back(getSeat(rowcol));
    });
    return seats;
}

const vector<pair<shared_ptr<Seat>, shared_ptr<Seat>>>
Seats::__getBishopObs_MoveSeats(bool isBottom, const Seat& fseat) const
{
    int frow { fseat.row() }, fcol { fseat.col() };
    vector<pair<shared_ptr<Seat>, shared_ptr<Seat>>> obs_MoveSeats {};
    vector<pair<pair<int, int>, pair<int, int>>> obs_MoveRowcols {
        { { frow - 1, fcol - 1 }, { frow - 2, fcol - 2 } },
        { { frow - 1, fcol + 1 }, { frow - 2, fcol + 2 } },
        { { frow + 1, fcol - 1 }, { frow + 2, fcol - 2 } },
        { { frow + 1, fcol + 1 }, { frow + 2, fcol + 2 } }
    };
    int rowLow { isBottom ? RowLowIndex_ : RowUpLowIndex_ },
        rowUp { isBottom ? RowLowUpIndex_ : RowUpIndex_ };
    for_each(obs_MoveRowcols.begin(), obs_MoveRowcols.end(),
        [&](const pair<pair<int, int>, pair<int, int>>& obs_Moverowcol) {
            if (obs_Moverowcol.second.first >= rowLow
                && obs_Moverowcol.second.first <= rowUp
                && obs_Moverowcol.second.second >= ColLowIndex_
                && obs_Moverowcol.second.second <= ColUpIndex_)
                obs_MoveSeats.push_back(
                    { getSeat(obs_Moverowcol.first),
                        getSeat(obs_Moverowcol.second) });
        });
    return obs_MoveSeats;
}

const vector<pair<shared_ptr<Seat>, shared_ptr<Seat>>>
Seats::__getKnightObs_MoveSeats(const Seat& fseat) const
{
    int frow { fseat.row() }, fcol { fseat.col() };
    vector<pair<shared_ptr<Seat>, shared_ptr<Seat>>> obs_MoveSeats {};
    vector<pair<pair<int, int>, pair<int, int>>> obs_MoveRowcols {
        { { frow - 1, fcol }, { frow - 2, fcol - 1 } },
        { { frow - 1, fcol }, { frow - 2, fcol + 1 } },
        { { frow, fcol - 1 }, { frow - 1, fcol - 2 } },
        { { frow, fcol + 1 }, { frow - 1, fcol + 2 } },
        { { frow, fcol - 1 }, { frow + 1, fcol - 2 } },
        { { frow, fcol + 1 }, { frow + 1, fcol + 2 } },
        { { frow + 1, fcol }, { frow + 2, fcol - 1 } },
        { { frow + 1, fcol }, { frow + 2, fcol + 1 } }
    };
    for_each(obs_MoveRowcols.begin(), obs_MoveRowcols.end(),
        [&](const pair<pair<int, int>, pair<int, int>>& obs_Moverowcol) {
            if (obs_Moverowcol.second.first >= RowLowIndex_
                && obs_Moverowcol.second.first <= RowUpIndex_
                && obs_Moverowcol.second.second >= ColLowIndex_
                && obs_Moverowcol.second.second <= ColUpIndex_)
                obs_MoveSeats.push_back(
                    { getSeat(obs_Moverowcol.first),
                        getSeat(obs_Moverowcol.second) });
        });
    return obs_MoveSeats;
}

const vector<shared_ptr<Seat>>
Seats::__getNonObstacleSeats(const vector<pair<shared_ptr<Seat>, shared_ptr<Seat>>>& obs_MoveSeats) const
{
    vector<shared_ptr<Seat>> seats {};
    for_each(obs_MoveSeats.begin(), obs_MoveSeats.end(),
        [&](const pair<shared_ptr<Seat>, shared_ptr<Seat>>& obs_MoveSeat) {
            if (!obs_MoveSeat.first->piece())
                seats.push_back(obs_MoveSeat.second);
        });
    return seats;
}

const vector<vector<shared_ptr<Seat>>>
Seats::__getRookCannonMoveSeat_Lines(const Seat& fseat) const
{
    int frow { fseat.row() }, fcol { fseat.col() };
    vector<vector<shared_ptr<Seat>>> seat_Lines(4);
    for (int col = fcol - 1; col >= ColLowIndex_; --col)
        seat_Lines[0].push_back(getSeat(frow, col));
    for (int col = fcol + 1; col <= ColUpIndex_; ++col)
        seat_Lines[1].push_back(getSeat(frow, col));
    for (int row = frow - 1; row >= RowLowIndex_; --row)
        seat_Lines[2].push_back(getSeat(row, fcol));
    for (int row = frow + 1; row <= RowUpIndex_; ++row)
        seat_Lines[3].push_back(getSeat(row, fcol));
    return seat_Lines;
}

}