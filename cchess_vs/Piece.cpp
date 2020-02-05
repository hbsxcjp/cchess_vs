#include "Piece.h"
#include "Seat.h"

namespace PieceSpace {

/* ===== Piece start. ===== */
Piece::Piece(wchar_t ch)
    : ch_{ ch }
    , name_{ PieceManager::getName(ch) }
    , color_{ PieceManager::getColor(ch) }
    , kind_{ PieceManager::getKindFromCh(ch) }
{
}

const wstring Piece::toString() const
{
    wstringstream wss{};
    wss << (color() == PieceColor::RED ? L'+' : L'*') << static_cast<int>(kind())
        << ch() << name() << PieceManager::getPrintName(*this);
    return wss.str();
}
/* ===== Piece end. ===== */

/* ===== Pieces start. ===== */
Pieces::Pieces()
{
    const wstring chars = { L"KAABBNNRRCCPPPPPkaabbnnrrccppppp" };
    for (auto& ch : chars)
        allPieces_.push_back(make_shared<Piece>(ch));
}

const SPiece& Pieces::getOtherPiece(const SPiece& piece) const
{
    if (!piece)
        return piece;
    auto first = allPieces_.begin(), last = allPieces_.end();
    return allPieces_.at((distance(first, find(first, last, piece)) + PIECENUM / 2) % PIECENUM);
}

const vector<SPiece> Pieces::getBoardPieces(const wstring& pieceChars) const
{
    vector<SPiece> pieces(SEATNUM);
    if (pieceChars.size() == SEATNUM) {
        vector<bool> used(allPieces_.size(), false);
        int pieceIndex{ 0 }, allPiecesSize = used.size();
        for_each(pieceChars.begin(), pieceChars.end(),
            [&](wchar_t ch) {
                if (ch != PieceManager::nullChar())
                    for (int index = 0; index < allPiecesSize; ++index)
                        if (allPieces_[index]->ch() == ch && !used[index]) {
                            pieces[pieceIndex] = allPieces_[index];
                            used[index] = true;
                            break;
                        }
                ++pieceIndex;
            });
    }
    return pieces;
}

const wstring Pieces::toString() const
{
    wstringstream wss{};
    for_each(allPieces_.begin(), allPieces_.end(),
        [&](const SPiece& piece) {
            wss << piece->toString() << L' ';
        });
    return wss.str();
}
/* ===== Pieces end. ===== */

/* ===== PieceManager start. ===== */
const wstring PieceManager::chChars_{ L"KABNRCPkabnrcp" };
const wstring PieceManager::preChars_{ L"ǰ�к�" };
const wstring PieceManager::nameChars_{ L"˧����ʿ�������ڱ���" };
const wstring PieceManager::movChars_{ L"��ƽ��" };
const map<PieceColor, wstring> PieceManager::numChars_{
    { PieceColor::RED, L"һ�����������߰˾�" },
    { PieceColor::BLACK, L"������������������" }
};
const wstring PieceManager::ICCSChars_{ L"abcdefghi" };
const wstring PieceManager::FirstFEN_{ L"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR" };
/* ===== PieceManager end. ===== */
}