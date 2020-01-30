#include "Seat.h"
#include "Piece.h"

namespace SeatSpace {

/* ===== Seat start. ===== */
Seat::Seat(int row, int col)
    : row_{ row }
    , col_{ col }
{
}

bool Seat::isSameColor(const SSeat& seat) const
{
    auto& piece = seat->piece();
    return piece && piece->color() == piece_->color();
}

const SPiece& Seat::movTo(SSeat& tseat, const SPiece& eatPiece)
{
    auto& tpiece = tseat->piece();
    tseat->setPiece(this->piece());
    setPiece(eatPiece);
    return tpiece;
}

const wstring Seat::toString() const
{
    wstringstream wss{};
    wss << row_ << col_ << L'&' << (piece_ ? piece_->name() : L'_'); //<< boolalpha << setw(2) <<
    return wss.str();
}
/* ===== Seat end. ===== */

/* ===== Seats start. ===== */
Seats::Seats()
{
    for (auto& rowcol_pair : SeatManager::getAllRowcols())
        allSeats_.push_back(make_shared<Seat>(rowcol_pair.first, rowcol_pair.second));
}

const SSeat& Seats::getSeat(int row, int col) const
{
    return allSeats_.at(SeatManager::getIndex_rc(row, col));
}

const SSeat& Seats::getSeat(int rowcol) const
{
    return allSeats_.at(SeatManager::getIndex_rc(rowcol));
}

const SSeat& Seats::getKingSeat(bool isBottom) const
{
    for (auto& rowcol : SeatManager::getKingRowcols(isBottom)) {
        auto& seat = getSeat(rowcol);
        auto& piece = seat->piece();
        if (piece && PieceManager::isKing(piece->name()))
            return seat;
    }
    throw runtime_error("将（帅）不在棋盘上面!");
}

SSeat_vector Seats::putSeats(bool isBottom, const SPiece& piece) const
{
    switch (piece->kind()) {
    case PieceKind::KING:
        return getKingSeats(isBottom);
    case PieceKind::ADVISOR:
        return getAdvisorSeats(isBottom);
    case PieceKind::BISHOP:
        return getBishopSeats(isBottom);
    case PieceKind::PAWN:
        return getPawnSeats(isBottom);
    default: // KNIGHT ROOK CANNON
        break;
    }
    return getAllSeats();
}

SSeat_vector Seats::getMoveSeats(bool isBottom, const SSeat& fseat) const
{
    //assert(fseat->piece()); // 该位置需有棋子，由调用者board来保证？
    switch (fseat->piece()->kind()) {
    case PieceKind::ROOK:
        return getRookMoveSeats(fseat);
    case PieceKind::KNIGHT:
        return getKnightMoveSeats(isBottom, fseat);
    case PieceKind::CANNON:
        return getCannonMoveSeats(fseat);
    case PieceKind::BISHOP:
        return getBishopMoveSeats(isBottom, fseat);
    case PieceKind::ADVISOR:
        return getAdvisorMoveSeats(isBottom, fseat);
    case PieceKind::PAWN:
        return getPawnMoveSeats(isBottom, fseat);
    case PieceKind::KING:
        return getKingMoveSeats(isBottom, fseat);
    default:
        break;
    };
    return SSeat_vector{};
}

SSeat_vector Seats::getLiveSeats(PieceColor color, wchar_t name, int col, bool getStronge) const
{
    SSeat_vector seats{};
    copy_if(allSeats_.begin(), allSeats_.end(), back_inserter(seats),
        [&](const SSeat& seat) {
            auto& piece = seat->piece();
            return (piece
                && color == piece->color()
                && (name == BLANKNAME || name == piece->name())
                && (col == BLANKCOL || col == seat->col())
                && (!getStronge || PieceManager::isStronge(piece->name())));
        });
    return seats;
}

SSeat_vector Seats::getSortPawnLiveSeats(bool isBottom,
    PieceColor color, wchar_t name) const
{
    // 最多5个兵
    SSeat_vector pawnSeats{ getLiveSeats(color, name) }, seats{};
    // 按列建立字典，按列排序
    map<int, SSeat_vector> colSeats{};
    for_each(pawnSeats.begin(), pawnSeats.end(),
        [&](const SSeat& seat) {
            colSeats[isBottom ? -seat->col() : seat->col()].push_back(seat);
        }); // isBottom则列倒序,每列位置倒序

    // 整合成一个数组
    for_each(colSeats.begin(), colSeats.end(),
        [&](const pair<int, SSeat_vector>& colSeat) {
            if (colSeat.second.size() > 1) // 筛除只有一个位置的列
                copy(colSeat.second.begin(), colSeat.second.end(), back_inserter(seats));
        }); //按列存入
    return seats;
}

void Seats::setPieces(const vector<SPiece>& boardPieces)
{
    int index{ 0 };
    for_each(allSeats_.begin(), allSeats_.end(),
        [&](const SSeat& seat) { seat->setPiece(boardPieces[index++]); });
}

void Seats::changeSide(const ChangeType ct, const shared_ptr<Pieces>& pieces)
{
    vector<SPiece> boardPieces{};
    auto changeRowcol = (ct == ChangeType::ROTATE ? &SeatManager::getRotate : &SeatManager::getSymmetry);
    for_each(allSeats_.begin(), allSeats_.end(),
        [&](const SSeat& seat) {
            boardPieces.push_back(ct == ChangeType::EXCHANGE
                    ? pieces->getOtherPiece(seat->piece())
                    : getSeat(changeRowcol(seat->rowcol()))->piece());
        });
    setPieces(boardPieces);
}

const wstring Seats::getPieceChars() const
{
    wstringstream wss{};
    for_each(allSeats_.begin(), allSeats_.end(),
        [&](const SSeat& seat) {
            wss << (seat->piece() ? seat->piece()->ch() : PieceManager::nullChar());
        });
    return wss.str();
}

const wstring Seats::toString() const
{
    wstringstream wss{};
    for_each(allSeats_.begin(), allSeats_.end(),
        [&](const SSeat& seat) {
            wss << seat->toString() << L' ';
        });
    return wss.str();
}

vector<SSeat> Seats::getAllSeats() const
{
    return allSeats_;
}

SSeat_vector Seats::getKingSeats(bool isBottom) const
{
    return __getSeats(SeatManager::getKingRowcols(isBottom));
}

SSeat_vector Seats::getAdvisorSeats(bool isBottom) const
{
    return __getSeats(SeatManager::getAdvisorRowcols(isBottom));
}

SSeat_vector Seats::getBishopSeats(bool isBottom) const
{
    return __getSeats(SeatManager::getBishopRowcols(isBottom));
}

SSeat_vector Seats::getPawnSeats(bool isBottom) const
{
    return __getSeats(SeatManager::getPawnRowcols(isBottom));
}

vector<SSeat> Seats::getKingMoveSeats(bool isBottom, const SSeat& fseat) const
{
    return __getMoveSeats(SeatManager::getKingMoveRowcols(isBottom, fseat->row(), fseat->col()), fseat);
}

vector<SSeat> Seats::getAdvisorMoveSeats(bool isBottom, const SSeat& fseat) const
{
    return __getMoveSeats(SeatManager::getAdvisorMoveRowcols(isBottom, fseat->row(), fseat->col()), fseat);
}

vector<SSeat> Seats::getBishopMoveSeats(bool isBottom, const SSeat& fseat) const
{
    return __getMoveSeats(__getNonObs_MoveRowcols(isBottom, fseat, *SeatManager::getBishopObs_MoveRowcols), fseat);
}

vector<SSeat> Seats::getKnightMoveSeats(bool isBottom, const SSeat& fseat) const
{
    return __getMoveSeats(__getNonObs_MoveRowcols(isBottom, fseat, *SeatManager::getKnightObs_MoveRowcols), fseat);
}

vector<SSeat> Seats::getRookMoveSeats(const SSeat& fseat) const
{
    return __getMoveSeats(__getRook_MoveRowcols(fseat), fseat);
}

vector<SSeat> Seats::getCannonMoveSeats(const SSeat& fseat) const
{
    return __getMoveSeats(__getCannon_MoveRowcols(fseat), fseat);
}

vector<SSeat> Seats::getPawnMoveSeats(bool isBottom, const SSeat& fseat) const
{
    return __getMoveSeats(SeatManager::getPawnMoveRowcols(isBottom, fseat->row(), fseat->col()), fseat);
}

vector<SSeat> Seats::__getSeats(const vector<pair<int, int>>& rowcol_pairs) const
{
    vector<SSeat> seats{};
    for_each(rowcol_pairs.begin(), rowcol_pairs.end(),
        [&](const pair<int, int>& rowcol_pair) {
            seats.push_back(getSeat(rowcol_pair));
        });
    return seats;
}

vector<SSeat> Seats::__getMoveSeats(const vector<pair<int, int>>& rowcol_pairs, const SSeat& fseat) const
{
    vector<SSeat> moveSeats{};
    for_each(rowcol_pairs.begin(), rowcol_pairs.end(),
        [&](const pair<int, int>& rowcol_pair) {
            auto& seat = getSeat(rowcol_pair);
            if (!fseat->isSameColor(seat)) // 非同一颜色
                moveSeats.push_back(seat);
        });
    return moveSeats;
}

const vector<pair<int, int>>
Seats::__getNonObs_MoveRowcols(bool isBottom, const SSeat& fseat,
    const vector<pair<pair<int, int>, pair<int, int>>> getObs_MoveRowcols(bool, int, int)) const
{
    vector<pair<int, int>> rowcols{};
    auto& obs_MoveRowcols = getObs_MoveRowcols(isBottom, fseat->row(), fseat->col());
    for_each(obs_MoveRowcols.begin(), obs_MoveRowcols.end(),
        [&](const pair<pair<int, int>, pair<int, int>>& obs_Moverowcol) {
            if (!getSeat(obs_Moverowcol.first)->piece()) // 该位置无棋子
                rowcols.push_back(obs_Moverowcol.second);
        });
    return rowcols;
}

const vector<pair<int, int>>
Seats::__getRook_MoveRowcols(const SSeat& fseat) const
{
    vector<pair<int, int>> rowcols{};
    for (auto& rowcolpair_Line :
        SeatManager::getRookCannonMoveRowcol_Lines(fseat->row(), fseat->col())) {
        for (auto& rowcol_pair : rowcolpair_Line) {
            rowcols.push_back(rowcol_pair);
            if (getSeat(rowcol_pair)->piece()) // 该位置有棋子
                break;
        }
    }
    return rowcols;
}

const vector<pair<int, int>>
Seats::__getCannon_MoveRowcols(const SSeat& fseat) const
{
    vector<pair<int, int>> rowcols{};
    for (auto& rowcolpair_Line :
        SeatManager::getRookCannonMoveRowcol_Lines(fseat->row(), fseat->col())) {
        bool isSkip = false; // 是否已跳棋子的标志
        for (auto& rowcol_pair : rowcolpair_Line) {
            auto& piece = getSeat(rowcol_pair)->piece();
            if (!isSkip) {
                if (!piece) // 该位置无棋子
                    rowcols.push_back(rowcol_pair);
                else
                    isSkip = true;
            } else if (piece) { // 该位置有棋子
                rowcols.push_back(rowcol_pair);
                break;
            }
        }
    }
    return rowcols;
}

const vector<pair<int, int>> SeatManager::getRowCols(const SSeat_vector& seats)
{
    vector<pair<int, int>> rowcols{};
    for_each(seats.begin(), seats.end(),
        [&](const SSeat& seat) {
            rowcols.emplace_back(seat->row(), seat->col());
        });
    return rowcols;
}

const vector<pair<int, int>> SeatManager::getAllRowcols()
{
    vector<pair<int, int>> rowcols{};
    for (int row = 0; row < BOARDROWNUM; ++row)
        for (int col = 0; col < BOARDCOLNUM; ++col)
            rowcols.emplace_back(row, col);
    return rowcols;
}

const vector<pair<int, int>> SeatManager::getKingRowcols(bool isBottom)
{
    vector<pair<int, int>> rowcols{};
    int rowLow{ isBottom ? RowLowIndex_ : RowUpMidIndex_ },
        rowUp{ isBottom ? RowLowMidIndex_ : RowUpIndex_ };
    for (int row = rowLow; row <= rowUp; ++row)
        for (int col = ColMidLowIndex_; col <= ColMidUpIndex_; ++col)
            rowcols.emplace_back(row, col);
    return rowcols;
}

const vector<pair<int, int>> SeatManager::getAdvisorRowcols(bool isBottom)
{
    vector<pair<int, int>> rowcols{};
    int rowLow{ isBottom ? RowLowIndex_ : RowUpMidIndex_ },
        rowUp{ isBottom ? RowLowMidIndex_ : RowUpIndex_ }, rmd{ isBottom ? 1 : 0 }; // 行列和的奇偶值
    for (int row = rowLow; row <= rowUp; ++row)
        for (int col = ColMidLowIndex_; col <= ColMidUpIndex_; ++col)
            if ((col + row) % 2 == rmd)
                rowcols.emplace_back(row, col);
    return rowcols;
}

const vector<pair<int, int>> SeatManager::getBishopRowcols(bool isBottom)
{
    vector<pair<int, int>> rowcols{};
    int rowLow{ isBottom ? RowLowIndex_ : RowUpLowIndex_ },
        rowUp{ isBottom ? RowLowUpIndex_ : RowUpIndex_ };
    for (int row = rowLow; row <= rowUp; row += 2)
        for (int col = ColLowIndex_; col <= ColUpIndex_; col += 2) {
            int gap{ row - col };
            if ((isBottom && (gap == 2 || gap == -2 || gap == -6))
                || (!isBottom && (gap == 7 || gap == 3 || gap == -1)))
                rowcols.emplace_back(row, col);
        }
    return rowcols;
}

const vector<pair<int, int>> SeatManager::getPawnRowcols(bool isBottom)
{
    vector<pair<int, int>> rowcols{};
    int lfrow{ isBottom ? RowLowUpIndex_ - 1 : RowUpLowIndex_ },
        ufrow{ isBottom ? RowLowUpIndex_ : RowUpLowIndex_ + 1 },
        ltrow{ isBottom ? RowUpLowIndex_ : RowLowIndex_ },
        utrow{ isBottom ? RowUpIndex_ : RowLowUpIndex_ };
    for (int row = lfrow; row <= ufrow; ++row)
        for (int col = ColLowIndex_; col <= ColUpIndex_; col += 2)
            rowcols.emplace_back(row, col);
    for (int row = ltrow; row <= utrow; ++row)
        for (int col = ColLowIndex_; col <= ColUpIndex_; ++col)
            rowcols.emplace_back(row, col);
    return rowcols;
}

const vector<pair<int, int>>
SeatManager::getKingMoveRowcols(bool isBottom, int frow, int fcol)
{
    vector<pair<int, int>> rowcols{
        { frow, fcol - 1 }, { frow, fcol + 1 },
        { frow - 1, fcol }, { frow + 1, fcol }
    };
    int rowLow{ isBottom ? RowLowIndex_ : RowUpMidIndex_ },
        rowUp{ isBottom ? RowLowMidIndex_ : RowUpIndex_ };
    auto pos = remove_if(rowcols.begin(), rowcols.end(),
        [&](const pair<int, int>& rowcol) {
            return !(rowcol.first >= rowLow && rowcol.first <= rowUp
                && rowcol.second >= ColMidLowIndex_ && rowcol.second <= ColMidUpIndex_);
        });
    return vector<pair<int, int>>{ rowcols.begin(), pos };
}

const vector<pair<int, int>>
SeatManager::getAdvisorMoveRowcols(bool isBottom, int frow, int fcol)
{
    vector<pair<int, int>> rowcols{
        { frow - 1, fcol - 1 }, { frow - 1, fcol + 1 },
        { frow + 1, fcol - 1 }, { frow + 1, fcol + 1 }
    };
    int rowLow{ isBottom ? RowLowIndex_ : RowUpMidIndex_ },
        rowUp{ isBottom ? RowLowMidIndex_ : RowUpIndex_ };
    auto pos = remove_if(rowcols.begin(), rowcols.end(),
        [&](const pair<int, int>& rowcol) {
            return !(rowcol.first >= rowLow && rowcol.first <= rowUp
                && rowcol.second >= ColMidLowIndex_ && rowcol.second <= ColMidUpIndex_);
        });
    return vector<pair<int, int>>{ rowcols.begin(), pos };
}

const vector<pair<pair<int, int>, pair<int, int>>>
SeatManager::getBishopObs_MoveRowcols(bool isBottom, int frow, int fcol)
{
    vector<pair<pair<int, int>, pair<int, int>>> obs_MoveRowcols{
        { { frow - 1, fcol - 1 }, { frow - 2, fcol - 2 } },
        { { frow - 1, fcol + 1 }, { frow - 2, fcol + 2 } },
        { { frow + 1, fcol - 1 }, { frow + 2, fcol - 2 } },
        { { frow + 1, fcol + 1 }, { frow + 2, fcol + 2 } }
    };
    int rowLow{ isBottom ? RowLowIndex_ : RowUpLowIndex_ },
        rowUp{ isBottom ? RowLowUpIndex_ : RowUpIndex_ };
    auto pos = remove_if(obs_MoveRowcols.begin(), obs_MoveRowcols.end(),
        [&](const pair<pair<int, int>, pair<int, int>>& obs_Moverowcol) {
            return (obs_Moverowcol.first.first < rowLow
                || obs_Moverowcol.first.first > rowUp
                || obs_Moverowcol.first.second < ColLowIndex_
                || obs_Moverowcol.first.second > ColUpIndex_);
        });
    return vector<pair<pair<int, int>, pair<int, int>>>{ obs_MoveRowcols.begin(), pos };
}

const vector<pair<pair<int, int>, pair<int, int>>>
SeatManager::getKnightObs_MoveRowcols(bool isBottom, int frow, int fcol)
{
    vector<pair<pair<int, int>, pair<int, int>>> obs_MoveRowcols{
        { { frow - 1, fcol }, { frow - 2, fcol - 1 } },
        { { frow - 1, fcol }, { frow - 2, fcol + 1 } },
        { { frow, fcol - 1 }, { frow - 1, fcol - 2 } },
        { { frow, fcol + 1 }, { frow - 1, fcol + 2 } },
        { { frow, fcol - 1 }, { frow + 1, fcol - 2 } },
        { { frow, fcol + 1 }, { frow + 1, fcol + 2 } },
        { { frow + 1, fcol }, { frow + 2, fcol - 1 } },
        { { frow + 1, fcol }, { frow + 2, fcol + 1 } }
    };
    auto pos = remove_if(obs_MoveRowcols.begin(), obs_MoveRowcols.end(),
        [&](const pair<pair<int, int>, pair<int, int>>& obs_Moverowcol) {
            return (obs_Moverowcol.first.first < RowLowIndex_
                || obs_Moverowcol.first.first > RowUpIndex_
                || obs_Moverowcol.first.second < ColLowIndex_
                || obs_Moverowcol.first.second > ColUpIndex_);
        });
    return vector<pair<pair<int, int>, pair<int, int>>>{ obs_MoveRowcols.begin(), pos };
}

const vector<vector<pair<int, int>>>
SeatManager::getRookCannonMoveRowcol_Lines(int frow, int fcol)
{
    vector<vector<pair<int, int>>> rowcol_Lines(4);
    for (int col = fcol - 1; col >= ColLowIndex_; --col)
        rowcol_Lines[0].emplace_back(frow, col);
    for (int col = fcol + 1; col <= ColUpIndex_; ++col)
        rowcol_Lines[1].emplace_back(frow, col);
    for (int row = frow - 1; row >= RowLowIndex_; --row)
        rowcol_Lines[2].emplace_back(row, fcol);
    for (int row = frow + 1; row <= RowUpIndex_; ++row)
        rowcol_Lines[3].emplace_back(row, fcol);
    return rowcol_Lines;
}

const vector<pair<int, int>>
SeatManager::getPawnMoveRowcols(bool isBottom, int frow, int fcol)
{
    vector<pair<int, int>> rowcols{};
    int row{}, col{};
    if ((isBottom && (row = frow + 1) <= RowUpIndex_)
        || (!isBottom && (row = frow - 1) >= RowLowIndex_))
        rowcols.emplace_back(row, fcol);
    if (isBottom == (frow > RowLowUpIndex_)) { // 兵已过河
        if ((col = fcol - 1) >= ColLowIndex_)
            rowcols.emplace_back(frow, col);
        if ((col = fcol + 1) <= ColUpIndex_)
            rowcols.emplace_back(frow, col);
    }
    return rowcols;
}
/* ===== Seats end. ===== */

const wstring getSeatsStr(const vector<SSeat>& seats)
{
    wstringstream wss{};
    wss << seats.size() << L"个: ";
    for_each(seats.begin(), seats.end(),
        [&](const SSeat& seat) {
            wss << seat->toString() << L' ';
        });
    return wss.str();
}

const wstring getRowColsStr(const vector<pair<int, int>>& rowcols)
{
    wstringstream wss{};
    wss << rowcols.size() << L"个: ";
    for_each(rowcols.begin(), rowcols.end(),
        [&](const pair<int, int>& rowcol) {
            wss << rowcol.first << rowcol.second << L' ';
        });
    return wss.str();
}
}