#include "Board.h"
#include "Piece.h"
#include "Seat.h"

namespace BoardSpace {

/* ===== Board start. ===== */
Board::Board(const wstring& pieceChars)
    : bottomColor_{ PieceColor::RED }
    , pieces_{ make_shared<Pieces>() }
    , seats_{ make_shared<Seats>() } // make_shared:动态分配内存，初始化对象并指向它
{
}

bool Board::isKilled(PieceColor color) const
{
    bool isBottom{ isBottomSide(color) };
    PieceColor othColor = PieceManager::getOtherColor(color);
    //SSeat kingSeat{ pieces_->getKingPiece(color)->seat() },
    //    othKingSeat{ pieces_->getKingPiece(othColor)->seat() };
    SSeat kingSeat{ seats_->getKingSeat(isBottom) },
        othKingSeat{ seats_->getKingSeat(!isBottom) };
    int fcol{ kingSeat->col() };
    if (fcol == othKingSeat->col()) {
        int krow{ kingSeat->row() }, orow{ othKingSeat->row() },
            lrow{ isBottom ? krow : orow }, urow{ isBottom ? orow : krow };
        for (int row = lrow + 1; row < urow; ++row) {
            if (!seats_->getSeat(row, fcol)->piece()) // 空棋子则继续循环
                continue;
            return true; // 全部是空棋子，则将帅对面
        }
    }
    // '获取某方可杀将棋子全部可走的位置
    for (auto& fseat : seats_->getLiveSeats(othColor, BLANKNAME, BLANKCOL, true)) {
        auto mvSeats = seats_->getMoveSeats(isBottom, fseat);
        if (!mvSeats.empty() && find(mvSeats.begin(), mvSeats.end(), kingSeat) != mvSeats.end()) // 对方强子可走位置有本将位置
            return true;
    }
    return false;
}

bool Board::isDied(PieceColor color) const
{
    for (auto& fseat : seats_->getLiveSeats(color))
        if (!__getCanMoveSeats(fseat).empty()) // 本方还有棋子可以走
            return false;
    return true;
}

SSeat_pair Board::getSeatPair(int frow, int fcol, int trow, int tcol) const
{
    return seats_->getSeatPair(frow, fcol, trow, tcol);
}

SSeat_pair Board::getSeatPair(int frowcol, int trowcol) const
{
    return seats_->getSeatPair(frowcol, trowcol);
}

SSeat_pair Board::getSeatPair(const wstring& str, RecFormat fmt) const
{
    return ((fmt == RecFormat::PGN_ZH || fmt == RecFormat::PGN_CC)
            ? __getSeatPairFromZhStr(str)
            : getSeatPair(
                  PieceManager::getRowFromICCSChar(str.at(1)),
                  PieceManager::getColFromICCSChar(str.at(0)),
                  PieceManager::getRowFromICCSChar(str.at(3)),
                  PieceManager::getColFromICCSChar(str.at(2))));
}

template <typename From_T1, typename From_T2>
const vector<pair<int, int>> Board::getCanMoveRowCols(From_T1 arg1, From_T2 arg2) const
{
    return SeatManager::getRowCols(__getCanMoveSeats(__getSeat(arg1, arg2)));
}
template const vector<pair<int, int>> Board::getCanMoveRowCols(int arg1, int arg2) const;
template const vector<pair<int, int>> Board::getCanMoveRowCols(const wstring& arg1, RecFormat arg2) const;

const vector<pair<int, int>> Board::getLiveRowCols(PieceColor color) const
{
    return SeatManager::getRowCols(seats_->getLiveSeats(color));
}

void Board::setPieces(const wstring& pieceChars)
{
    seats_->setPieces(pieces_->getBoardPieces(pieceChars));
    __setBottomSide();
}

void Board::changeSide(const ChangeType ct)
{
    seats_->changeSide(ct, pieces_);
    __setBottomSide();
}

//(fseat, tseat)->中文纵线着法
const wstring Board::getZhStr(SSeat_pair seat_pair) const
{
    wstringstream wss{};
    auto &fseat = seat_pair.first, &tseat = seat_pair.second;
    const SPiece& fromPiece{ fseat->piece() };
    PieceColor color{ fromPiece->color() };
    wchar_t name{ fromPiece->name() };
    int fromRow{ fseat->row() }, fromCol{ fseat->col() },
        toRow{ tseat->row() }, toCol{ tseat->col() };
    bool isSameRow{ fromRow == toRow }, isBottom{ isBottomSide(color) };
    auto seats = seats_->getLiveSeats(color, name, fromCol);

    if (seats.size() > 1 && PieceManager::isStronge(name)) {
        if (PieceManager::isPawn(name))
            seats = seats_->getSortPawnLiveSeats(isBottom, color, name);
        wss << PieceManager::getPreChar(seats.size(), isBottom,
                   distance(seats.begin(), find(seats.begin(), seats.end(), fseat)))
            << name;
    } else //将帅, 仕(士),相(象): 不用“前”和“后”区别，因为能退的一定在前，能进的一定在后
        wss << name << PieceManager::getColChar(color, isBottom, fromCol);
    wss << PieceManager::getMovChar(isSameRow, isBottom, toRow > fromRow)
        << (PieceManager::isLineMove(name) && !isSameRow
                   ? PieceManager::getNumChar(color, abs(fromRow - toRow)) // 非同一行
                   : PieceManager::getColChar(color, isBottom, toCol));

    //assert(__getSeatPairFromZhStr(wss.str()) == seat_pair);

    return wss.str();
}

const wstring Board::getPieceChars() const
{
    return seats_->getPieceChars();
}

const wstring Board::toString() const
{ // 文本空棋盘
    wstring textBlankBoard{ L"┏━┯━┯━┯━┯━┯━┯━┯━┓\n"
                            "┃　│　│　│╲│╱│　│　│　┃\n"
                            "┠─┼─┼─┼─╳─┼─┼─┼─┨\n"
                            "┃　│　│　│╱│╲│　│　│　┃\n"
                            "┠─╬─┼─┼─┼─┼─┼─╬─┨\n"
                            "┃　│　│　│　│　│　│　│　┃\n"
                            "┠─┼─╬─┼─╬─┼─╬─┼─┨\n"
                            "┃　│　│　│　│　│　│　│　┃\n"
                            "┠─┴─┴─┴─┴─┴─┴─┴─┨\n"
                            "┃　　　　　　　　　　　　　　　┃\n"
                            "┠─┬─┬─┬─┬─┬─┬─┬─┨\n"
                            "┃　│　│　│　│　│　│　│　┃\n"
                            "┠─┼─╬─┼─╬─┼─╬─┼─┨\n"
                            "┃　│　│　│　│　│　│　│　┃\n"
                            "┠─╬─┼─┼─┼─┼─┼─╬─┨\n"
                            "┃　│　│　│╲│╱│　│　│　┃\n"
                            "┠─┼─┼─┼─╳─┼─┼─┼─┨\n"
                            "┃　│　│　│╱│╲│　│　│　┃\n"
                            "┗━┷━┷━┷━┷━┷━┷━┷━┛\n" }; // 边框粗线
    wstringstream wss{};
    // Pieces test
    wss << L"pieces_:\n"
        << pieces_->toString() << L"\n";
    // Seats test
    wss << L"seats_:\n"
        << seats_->toString() << L"\n";

    for (auto color : { PieceColor::BLACK, PieceColor::RED })
        for (auto& seat : seats_->getLiveSeats(color))
            textBlankBoard[(BOARDROWNUM - 1 - seat->row()) * 2 * (BOARDCOLNUM * 2) + seat->col() * 2]
                = PieceManager::getPrintName(*seat->piece());
    wss << textBlankBoard;
    return wss.str();
}

void Board::__setBottomSide()
{
    //auto kseat = pieces_->getKingPiece(PieceColor::RED)->seat();
    //assert(kseat);
    bottomColor_ = seats_->getKingSeat(true)->piece()->color();
}

const SSeat& Board::__getSeat(int row, int col) const
{
    return seats_->getSeat(row, col);
}

const SSeat& Board::__getSeat(const wstring& str, RecFormat fmt) const
{
    return ((fmt == RecFormat::PGN_ZH || fmt == RecFormat::PGN_CC)
            ? __getSeatFromZhStr(str)
            : seats_->getSeat(
                  PieceManager::getRowFromICCSChar(str.at(1)),
                  PieceManager::getColFromICCSChar(str.at(0))));
}

const SSeat& Board::__getSeatFromZhStr(const wstring& zhStr) const
{
    return __getSeatPairFromZhStr(zhStr).first;
}

SSeat_pair Board::__getSeatPairFromZhStr(const wstring& zhStr) const
{
    assert(zhStr.size() == 4);
    SSeat fseat{}, tseat{};
    SSeat_vector seats{};
    // 根据最后一个字符判断该着法属于哪一方
    PieceColor color{ PieceManager::getColorFromZh(zhStr.back()) };
    bool isBottom{ isBottomSide(color) };
    int index{}, movDir{ PieceManager::getMovNum(isBottom, zhStr.at(2)) };
    wchar_t name{ zhStr.front() };

    if (PieceManager::isPiece(name)) { // 首字符为棋子名
        seats = seats_->getLiveSeats(color, name,
            PieceManager::getCurIndex(isBottom,
                PieceManager::getNumIndex(color, zhStr.at(1)), BOARDCOLNUM));
        assert(seats.size() > 0);
        //# 排除：士、象同列时不分前后，以进、退区分棋子。移动方向为退时，修正index
        index = (seats.size() == 2 && movDir == -1) ? 1 : 0; //&& isAdvBish(name)
    } else {
        name = zhStr.at(1);
        seats = (PieceManager::isPawn(name)
                ? seats_->getSortPawnLiveSeats(isBottom, color, name)
                : seats_->getLiveSeats(color, name));
        index = PieceManager::getPreIndex(seats.size(), isBottom, zhStr.front());
    }

    assert(index <= static_cast<int>(seats.size()) - 1);
    fseat = seats.at(index);

    int numIndex{ PieceManager::getNumIndex(color, zhStr.back()) },
        toCol{ PieceManager::getCurIndex(isBottom, numIndex, BOARDCOLNUM) };
    if (PieceManager::isLineMove(name)) {
        int trow{ fseat->row() + movDir * (numIndex + 1) };
        tseat = movDir == 0 ? seats_->getSeat(fseat->row(), toCol) : seats_->getSeat(trow, fseat->col());
    } else { // 斜线走子：仕、相、马
        int colAway{ abs(toCol - fseat->col()) }, //  相距1或2列
            trow{ fseat->row()
                + movDir * ((PieceManager::isAdvisor(name) || PieceManager::isBishop(name)) ? colAway : (colAway == 1 ? 2 : 1)) };
        tseat = seats_->getSeat(trow, toCol);
    }
    //assert(zhStr == getZh(fseat, tseat));

    return make_pair(fseat, tseat);
}

SSeat_vector Board::__getCanMoveSeats(const SSeat& fseat) const
{
    assert(fseat->piece());
    PieceColor color{ fseat->piece()->color() };
    //SPiece toPiece;
    auto seats = seats_->getMoveSeats(isBottomSide(color), fseat);
    auto fseat_cp = fseat; // 新建一个非const的副本，供下面测试是否被将军使用 (参见:c++ Primer Page.192)
    auto pos = remove_if(seats.begin(), seats.end(),
        [&](SSeat& tseat) {
            // 移动棋子后，检测是否会被对方将军
            auto& eatPiece = fseat_cp->movTo(tseat);
            bool killed{ isKilled(color) };
            tseat->movTo(fseat_cp, eatPiece);
            return killed;
        });
    return SSeat_vector{ seats.begin(), pos };
}
/* ===== Board end. ===== */
}
