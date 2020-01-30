#include "Piece.h"
//#include "Board.h"
#include "Seat.h"
#include <algorithm>
#include <cassert>
//#include <iomanip>
#include <sstream>

using namespace std;
using namespace SeatSpace;
using namespace BoardSpace;

namespace PieceSpace {

Piece::Piece(const wchar_t ch)
    : ch_ { ch }
    , seat_ { nullptr }
{
}

const wchar_t Piece::name() const
{
    return Pieces::getName(ch_);
}

const PieceColor Piece::color() const
{
    return Pieces::getColor(ch_);
}

const vector<shared_ptr<Seat>> Piece::putSeats(bool isBottom, const Seats& seats) const
{
    auto thisName = name();
    if (Pieces::isKing(thisName))
        return seats.getKingSeats(isBottom);
    else if (Pieces::isAdvisor(thisName))
        return seats.getAdvisorSeats(isBottom);
    else if (Pieces::isBishop(thisName))
        return seats.getBishopSeats(isBottom);
    else if (Pieces::isPawn(thisName))
        return seats.getPawnSeats(isBottom);
    else
        return seats.getAllSeats();
}

const wstring Piece::toString() const
{
    wstringstream wss {};
    wss << (color() == PieceColor::RED ? L'+' : L'*')
        << ch() << Pieces::getPrintName(*this); //<< boolalpha
    return wss.str();
}

const shared_ptr<Piece>&
Pieces::getOtherPiece(const shared_ptr<Piece>& piece) const
{
    if (!piece)
        return piece;
    return allPieces_.at(
        (distance(allPieces_.begin(),
             find(allPieces_.begin(), allPieces_.end(), piece))
            + allPieces_.size() / 2)
        % allPieces_.size());
}

const vector<shared_ptr<Piece>>
Pieces::getBoardPieces(const wstring& pieceChars) const
{
    assert(pieceChars.size() == 90);
    vector<shared_ptr<Piece>> pieces(pieceChars.size());
    vector<bool> used(allPieces_.size(), false);
    int pieceIndex { 0 }, allPiecesSize = used.size();
    for_each(pieceChars.begin(), pieceChars.end(),
        [&](const wchar_t ch) {
            if (ch != Pieces::nullChar())
                for (int index = 0; index < allPiecesSize; ++index)
                    if (allPieces_[index]->ch() == ch && !used[index]) {
                        pieces[pieceIndex] = allPieces_[index];
                        used[index] = true;
                        break;
                    }
            ++pieceIndex;
        });
    return pieces;
}

const wstring Pieces::toString() const
{
    wstringstream wss {};
    for_each(allPieces_.begin(), allPieces_.end(),
        [&](const shared_ptr<Piece>& piece) {
            wss << piece->toString() << L' ';
        });
    return wss.str();
}

Pieces::Pieces()
{
    const wstring chars = { L"KAABBNNRRCCPPPPPkaabbnnrrccppppp" };
    for (auto& ch : chars)
        allPieces_.push_back(make_shared<Piece>(ch));
}

const wchar_t Pieces::getName(const wchar_t ch)
{
    const map<int, int> chIndex_nameIndex {
        { 0, 0 }, { 1, 2 }, { 2, 4 }, { 3, 6 }, { 4, 7 }, { 5, 8 }, { 6, 9 },
        { 7, 1 }, { 8, 3 }, { 9, 5 }, { 10, 6 }, { 11, 7 }, { 12, 8 }, { 13, 10 }
    };
    return nameChars_.at(chIndex_nameIndex.at(chChars_.find(ch)));
}

const wchar_t Pieces::getPrintName(const Piece& piece)
{
    const map<wchar_t, wchar_t>
        rcpName { { L'车', L'車' }, { L'马', L'馬' }, { L'炮', L'砲' } };
    const wchar_t name { piece.name() };
    return (piece.color() == PieceColor::BLACK
               && rcpName.find(name) != rcpName.end())
        ? rcpName.at(name)
        : name;
}

const PieceColor Pieces::getColor(const wchar_t ch)
{
    return islower(ch) ? PieceColor::BLACK : PieceColor::RED;
}

const PieceColor Pieces::getColorFromZh(const wchar_t numZh)
{
    return numChars_.at(PieceColor::RED).find(numZh) != wstring::npos
        ? PieceColor::RED
        : PieceColor::BLACK;
}

const int Pieces::getIndex(const int seatsLen, const bool isBottom, const wchar_t preChar)
{
    int index = __getPreChars(seatsLen).find(preChar);
    return isBottom ? seatsLen - 1 - index : index;
}

const wchar_t Pieces::getIndexChar(const int seatsLen, const bool isBottom, const int index)
{
    return __getPreChars(seatsLen).at(isBottom ? seatsLen - 1 - index : index);
}

const int Pieces::getCol(bool isBottom, const int num)
{
    return isBottom ? Seats::ColNum() - num : num - 1;
}

const wchar_t Pieces::getColChar(const PieceColor color, bool isBottom, const int col)
{
    return numChars_.at(color).at(isBottom ? Seats::ColNum() - col - 1 : col);
}

const wstring Pieces::chChars_ { L"KABNRCPkabnrcp" };
const wstring Pieces::preChars_ { L"前中后" };
const wstring Pieces::nameChars_ { L"帅将仕士相象马车炮兵卒" };
const wstring Pieces::movChars_ { L"退平进" };
const map<PieceColor, wstring> Pieces::numChars_ {
    { PieceColor::RED, L"一二三四五六七八九" },
    { PieceColor::BLACK, L"１２３４５６７８９" }
};
const wchar_t Pieces::nullChar_ { L'_' };
const wstring Pieces::ICCSChars_ { L"abcdefghi" };
const wstring Pieces::FENStr_ { L"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR" };

}