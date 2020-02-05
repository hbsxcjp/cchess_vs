// cchess_vs.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//
#include "Board.h"
#include "ChessManual.h"
#include "Tools.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <locale>

int main(int argc, char const* argv[])
{
    try {
        using namespace std::chrono;
        //std::locale loc = std::locale::global(std::locale(""));
        setlocale(LC_ALL, "");
        std::ios_base::sync_with_stdio(false);

        auto time0 = steady_clock::now();
        /*
    BoardSpace::Board board {};
    wstring fileName = L"d:\\����board.txt";
    wstring resultStr = board.test();
    Tools::writeFile(fileName, resultStr);
    //std::wcout << resultStr;

    //ChessManualSpace::ChessManual cm {};
    //*/
        std::wcout << chessmanual_test();
        /*
    if (argc == 7)
        InstanceSpace::testTransDir(std::stoi(argv[1]), std::stoi(argv[2]),
            std::stoi(argv[3]), std::stoi(argv[4]), std::stoi(argv[5]), std::stoi(argv[6]));
    else {
        //InstanceSpace::testTransDir(0, 2, 0, 6, 1, 6);
        //std::cout << "------------------------------------------------------------------" << std::endl;
        InstanceSpace::testTransDir(0, 2, 0, 1, 1, 6);
        InstanceSpace::testTransDir(0, 2, 1, 5, 5, 6);
        InstanceSpace::testTransDir(0, 2, 5, 6, 1, 2);
        //std::cout << "------------------------------------------------------------------" << std::endl;
        //InstanceSpace::testTransDir(0, 2, 2, 3, 1, 5);
    }
    //*/

        auto time_d = steady_clock::now() - time0;
        std::cout << "use time: " << duration_cast<milliseconds>(time_d).count() / 1000.0 << "s\n";

        //std::locale::global(loc);
        return 0;
    } catch (runtime_error err) {
        cout << err.what() << endl;
    }
    return -74;
}

// ���г���: Ctrl + F5 ����� >����ʼִ��(������)���˵�
// ���Գ���: F5 ����� >����ʼ���ԡ��˵�

// ����ʹ�ü���:
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
