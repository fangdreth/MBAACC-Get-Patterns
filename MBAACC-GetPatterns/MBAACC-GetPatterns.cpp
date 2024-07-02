// MBAACC-GetPatterns.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include "tlhelp32.h"
#include <stdio.h>
#include <string.h>
#include <psapi.h>
#include <iostream>
#include <chrono>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>

// add semicolon at the end of the map definition
// j2b and j2c
// remove fastfall and nekotech and 2222c and 22222c

using namespace std;

static std::map<std::string, int> CharacterValues_Map =
{
    {"CAOKO", 20}, {"FAOKO", 20}, {"HAOKO", 22},
    {"CTOHNO", 30}, {"FTOHNO", 30}, {"HTOHNO", 32},
    {"CHIME", 40}, {"FHIME", 40}, {"HHIME", 43},
    {"CNANAYA", 50}, {"FNANAYA", 50}, {"HNANAYA", 52},
    {"CKOUMA", 60}, {"FKOUMA", 60}, {"HKOUMA", 62},
    {"CMIYAKO", 100}, {"FMIYAKO", 100}, {"HMIYAKO", 102},
    {"CCIEL", 110}, {"FCIEL", 110}, {"HCIEL", 112},
    {"CSION", 120}, {"FSION", 120}, {"HSION", 122},
    {"CRIES", 130}, {"FRIES", 130}, {"HRIES", 132},
    {"CVSION", 140}, {"FVSION", 140}, {"HVSION", 142},
    {"CWARA", 150}, {"FWARA", 150}, {"HWARA", 152},
    {"CROA", 160}, {"FROA", 160}, {"HROA", 162},
    {"CMAIDS", 190}, {"FMAIDS", 190}, {"HMAIDS", 192},
    {"CAKIHA", 200}, {"FAKIHA", 200}, {"HAKIHA", 202},
    {"CARC", 210}, {"FARC", 210}, {"HARC", 212},
    {"CPCIEL", 220}, {"FPCIEL", 220}, {"HPCIEL", 222},
    {"CWARC", 230}, {"FWARC", 230}, {"HWARC", 232},
    {"CVAKIHA", 240}, {"FVAKIHA", 240}, {"HVAKIHA", 242},
    {"CMECH", 250}, {"FMECH", 250}, {"HMECH", 252},
    {"CSEIFUKU", 280}, {"FSEIFUKU", 280}, {"HSEIFUKU", 282},
    {"CSATSUKI", 290}, {"FSATSUKI", 290}, {"HSATSUKI", 292},
    {"CLEN", 300}, {"FLEN", 300}, {"HLEN", 302},
    {"CRYOUGI", 310}, {"FRYOUGI", 310}, {"HRYOUGI", 312},
    {"CWLEN", 320}, {"FWLEN", 320}, {"HWLEN", 322},
    {"CNERO", 330}, {"FNERO", 330}, {"HNERO", 332},
    {"CNAC", 340}, {"FNAC", 340}, {"HNAC", 342},
    {"CKOHAMECH", 380}, {"FKOHAMECH", 380}, {"HKOHAMECH", 382},
    {"CHISUI", 390}, {"FHISUI", 390}, {"HHISUI", 392},
    {"CNECO", 400}, {"FNECO", 400}, {"HNECO", 402},
    {"CKOHAKU", 410}, {"FKOHAKU", 410}, {"HKOHAKU", 412},
    {"CNECOMECH", 420}, {"FNECOMECH", 420}, {"HNECOMECH", 422}
};

static std::vector<std::string> vEveryPatternName =
{
    "6A", "4B", "6B", "3B", "4C", "6C", "3C",
    "j.6A", "j.6B", "j.6C", "j.2C",
    "A DIVEKICK", "B DIVEKICK", "C DIVEKICK",
    "BUNKER", "5D", "2D", "j.D", "DODGE", "AIR DODGE", "HEAT", "THROW", "AIR THROW", "FLOAT", "AIR FLOAT",
    "BACK DASH", "BACK AIRDASH", "FORWARD DASH", "FORWARD AIRDASH", "FASTFALL", "FORWARD SUPER JUMP",
    "623A", "623B", "623C",
    "j.623A", "j.623B", "j.623C",
    "22A", "22B", "22C", "22D", "2222C", "22222C",
    "j.22A", "j.22B", "j.22C",
    //"NEKO TECH",
    "214A", "214B", "214C", "214214C",
    "j.214A", "j.214B", "j.214C",
    "236A", "236B", "236C",
    "j.236A", "j.236B", "j.236C",
    "421A", "421B", "421C",
    "j.421A", "j.421B", "j.421C",
    "624A", "624B", "624C",
    "j.624A", "j.624B", "j.624C",
    "ARC DRIVE",
    //"DRUG INSTALL"
};

static std::vector<int> vPatternsToSkip =
{
    0,  //standing
    11, 10, 12, 13, 14, //walking
    35, 36, 37, 476 //jumping
    //, 385 //koha crawl
    //, 59 //catwalk
    //, 60, 61 //neco and nac crawl
};

DWORD GetBaseAddressByName(HANDLE pHandle, const wchar_t* name)
{
    HMODULE hMods[1024];
    DWORD cbNeeded;

    if (EnumProcessModules(pHandle, hMods, sizeof(hMods), &cbNeeded))
    {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
        {
            TCHAR szModName[MAX_PATH];
            if (GetModuleFileNameEx(pHandle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
            {
                std::wstring wstrModName = szModName;
                std::wstring wstrModContain = L"MBAA.exe";
                if (wstrModName.find(wstrModContain) != std::string::npos)
                {
                    return (DWORD)hMods[i];
                }
            }
        }
    }

    return 0;
}

HANDLE GetProcessByName(const wchar_t* name)
{
    DWORD pid = 0;

    // Create toolhelp snapshot.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);

    // Walkthrough all processes.
    if (Process32First(snapshot, &process))
    {
        do
        {
            // Compare process.szExeFile based on format of name, i.e., trim file path
            // trim .exe if necessary, etc.
            if (std::wcscmp((process.szExeFile), name) == 0)
            {
                pid = process.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &process));
    }

    CloseHandle(snapshot);

    if (pid != 0)
    {
        return OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    }

    return nullptr;
}

int main()
{
    const DWORD dwP1PatternRead = 0x155140;
    const DWORD dwRoundTime = 0x162A40;

    int nReadResult = 0;
    HANDLE hMBAAHandle = 0x0;
    DWORD dwBaseAddress = 0;
    int nPatternCandidate = 0;
    bool bSpaceHeld = false;
    bool bShiftHeld = false;

    hMBAAHandle = GetProcessByName(L"MBAA.exe");
    dwBaseAddress = GetBaseAddressByName(hMBAAHandle, L"MBAA.exe");

    ofstream fOutput;
    fOutput.open("Moves_Output.txt");

    for (auto const& [key, value] : CharacterValues_Map)
    {
        // Controls:
        // Shift - Skip the current move
        // Space - Confirm the pattern for the current move
        //         Pressing with no pattern selected yet counts as a skip

        // remember to modify the vPatternsToSkip vector if you use these
        //if (key.substr(0, 1) == "C") continue;  // skip C
        //if (key.substr(0, 1) == "H") continue;  // skip H
        //if (key.substr(0, 1) == "F") continue;  // skip F
        //if (key.find("LEN") == std::string::npos || key.find("W") != std::string::npos) continue; // only do len (and wlen)
        //if (key.find("NECO") == std::string::npos || key.find("MECH") != std::string::npos) continue; // only do neco (and necomech
        //if (key.find("NAC") == std::string::npos) continue; // only do NAC
        //if (key.find("CKOHA") == std::string::npos || key.find("MECH") != std::string::npos) continue; // only do ckoha

        fOutput << "const std::map<std::string, int> v" << key << "_Map = \n{\n";

        for (auto& xMove : vEveryPatternName)
        {
            system("cls");
            cout << key << endl << endl;
            cout << xMove << ": ";

            while (1)
            {
                if (GetKeyState(VK_SPACE) & 0x8000)
                {
                    if (!bSpaceHeld)
                    {
                        bSpaceHeld = true;
                        break;
                    }
                }
                else
                    bSpaceHeld = false;

                if (GetKeyState(VK_SHIFT) & 0x8000)
                {
                    if (!bShiftHeld)
                    {
                        bShiftHeld = true;
                        break;
                    }
                }
                else
                    bShiftHeld = false;

                ReadProcessMemory(hMBAAHandle, (LPVOID)(dwBaseAddress + dwP1PatternRead), &nReadResult, 4, 0);
                if (find(vPatternsToSkip.begin(), vPatternsToSkip.end(), nReadResult) == vPatternsToSkip.end())
                {
                    nPatternCandidate = nReadResult;
                    cout << nPatternCandidate << "\t\t[SPACE TO CONFIRM]";
                    
                    while (1)
                    {
                        // TODO
                        // space => break loop as-is
                        // timer == 0 => nPatternCandidate = 0 then break loop
                        ReadProcessMemory(hMBAAHandle, (LPVOID)(dwBaseAddress + dwRoundTime), &nReadResult, 4, 0);
                        if (nReadResult == 0)
                        {
                            system("cls");
                            cout << key << endl << endl;
                            cout << xMove << ": ";

                            nPatternCandidate = 0;
                            break;
                        }

                        if (GetKeyState(VK_SPACE) & 0x8000)
                        {
                            if (!bSpaceHeld)
                            {
                                bSpaceHeld = true;
                                break;
                            }
                        }
                        else
                            bSpaceHeld = false;

                        if (GetKeyState(VK_SHIFT) & 0x8000)
                        {
                            if (!bShiftHeld)
                            {
                                nPatternCandidate = -1;
                                bShiftHeld = true;
                                break;
                            }
                        }
                        else
                            bShiftHeld = false;
                    }

                    if (nPatternCandidate == -1)
                    {
                        break;
                    }

                    if (nPatternCandidate != 0)
                    {
                        fOutput << "\n\t{ \"" << xMove << "\", " << nPatternCandidate << " }, ";
                        break;
                    }
                }
            }
        }
    
        fOutput << "<delete this>\n};\n\n";
    }

    fOutput.close();
}
