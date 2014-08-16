// Ricefarm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Farm.h"

struct comparator
{
    bool operator() (riceVariety *first, riceVariety *second)
    {
        return (first->nonConflictingNeighbours.size() < second->nonConflictingNeighbours.size());
    }
}comparatorObj;

farm::farm(int i, int j, char *inFilePath, char *outFilePath) : rows(i), columns(j)
{
    farmGrid = new plot**[rows];
    for (int index=0; index<rows; ++index)
    {
        farmGrid[index] = new plot*[columns];
    }

    int count = 1;
    for (int hindex=0; hindex<rows; ++hindex)
    {
        for (int vindex=0; vindex<columns; ++vindex)
        {
            plot *newplot = new plot(hindex, vindex, count);
            farmGrid[hindex][vindex] = newplot;
            farmPlotList.push_back(newplot);
            count++;
        }
    }

    for (int hindex=0; hindex<rows; ++hindex)
    {
        for (int vindex=0; vindex<columns; ++vindex)
        {
            plot *plotObj = farmGrid[hindex][vindex];
            int row = plotObj->rowIndex;
            int col = plotObj->colIndex;

            vector<plot *> adjacenPlotList;
            if (row-1 >= 0 && col-1 >= 0)           adjacenPlotList.push_back(farmGrid[row-1][col-1]);
            if (row-1 >= 0)                         adjacenPlotList.push_back(farmGrid[row-1][col]);
            if (row-1 >= 0 && col+1 < columns)      adjacenPlotList.push_back(farmGrid[row-1][col+1]);
            if (col+1 < columns)                    adjacenPlotList.push_back(farmGrid[row][col+1]);
            if (row+1 < rows && col+1 < columns)    adjacenPlotList.push_back(farmGrid[row+1][col+1]);
            if (row+1 < rows)                       adjacenPlotList.push_back(farmGrid[row+1][col]);
            if (row+1 < rows && col-1 >= 0)         adjacenPlotList.push_back(farmGrid[row+1][col-1]);
            if (col-1 >= 0)                         adjacenPlotList.push_back(farmGrid[row][col-1]);

            farmPlotAdjacencyMap.insert(pair<plot *, vector<plot *> >(plotObj, adjacenPlotList));
        }
    }

    maxOccupiedPlots = 0;

    if (inFilePath == NULL)
    {
        inFilePath = "input.csv";
    }
    if (outFilePath == NULL)
    {
        outFilePath = "output.csv";
    }
    inputFilePath = inFilePath;
    outputFilePath = outFilePath;
}

farm::~farm()
{
    for (auto &obj : farmPlotAdjacencyMap)
    {
        delete obj.first;
    }

    for (auto &variety: riceVarietyList)
    {
        delete variety;
    }

    for (int index=0; index<rows; ++index)
    {
        delete [] farmGrid[index];
    }
    delete [] farmGrid;
}

void farm::populateRiceVarietyList()
{
    ifstream infile;
    infile.open(inputFilePath.c_str());
    if (!infile)
    {
        throw "ERROR: Input file '" + inputFilePath + "' could not be opened, check if it is present";
    }

    string line;
    getline(infile, line);

    if (_strcmpi(line.c_str(), "Variety,Start Date,End Date") != 0)
    {
        throw "ERROR: input file CSV header '" + line + "' is not as desired, check the sample";
    }

    int lineNumber = 2;
    for (line.clear();getline(infile, line);line.clear())
    {
        string name, sd, ed;
        size_t nameEnd = line.find_first_of(',', 0);
        if (nameEnd != string::npos)
        {
            name = line.substr(0, nameEnd);
        }
        else
        {
            throw "ERROR: input file CSV is corrupt at line " + lineNumber;
        }

        size_t sdEnd = line.find_first_of(',', nameEnd + 1);
        if (sdEnd != string::npos)
        {
            sd = line.substr(nameEnd + 1, sdEnd - nameEnd - 1);
        }
        else
        {
            throw "ERROR: input file CSV is corrupt at line " + lineNumber;
        }

        //size_t edEnd = line.find_first_of('\0', sdEnd + 1);
        //if (edEnd != string::npos)
        //{
        ed = line.substr(sdEnd + 1);
        //}
        //else
        //{
        //    throw "ERROR: input file CSV is corrupt at line " + lineNumber;
        //}
        ++lineNumber;

        riceVariety *variety = new riceVariety(name, sd, ed);
        riceVarietyList.push_back(variety);
    }
    infile.close();
}

void farm::populateNonConflictingNeighbours()
{
    for (auto &variety : riceVarietyList)
    {
        for (auto &runner : riceVarietyList)
        {
            if (runner == variety)
            {
                continue;
            }
            else if (!variety->pollinationConflicts(runner))
            {
                (variety)->nonConflictingNeighbours.insert(runner);
            }
        }
    }
}

void farm::sortListAsPerDesiredOrder()
{
    //sort the map in decreasing order of conflict
    //std::sort(riceVarietyList.begin(), riceVarietyList.end(), comparatorObj);
}

bool farm::recurseAndAssignVarietyToPlots(int currentPlotIndex)
{
    if (currentPlotIndex >= (int)farmPlotList.size())
    {
        return true;
    }

    bool allVarietyUsed = true;
    for (int varietyIndex = 0; varietyIndex < (int)riceVarietyList.size(); ++varietyIndex)
    {
        riceVariety *currentVariety = riceVarietyList[varietyIndex];
        if (currentVariety->used == true)
        {
            continue;
        }

        allVarietyUsed = false;
        if (willItConflict(currentPlotIndex, currentVariety))
        {
            continue;
        }

        farmPlotList[currentPlotIndex]->assignedVariety = currentVariety;
        currentVariety->used = true;

        bool retValue = recurseAndAssignVarietyToPlots(currentPlotIndex + 1);
        if (retValue == false)
        {
            farmPlotList[currentPlotIndex]->assignedVariety = NULL;
            currentVariety->used = false;
            continue;
        }
        else
        {
            return true;
        }
    }
    if (allVarietyUsed)
    {
        return true;
    }
    else
    {
        //farmPlotList[currentPlotIndex]->assignedVariety = NULL;
        //return recurseAndAssignVarietyToPlots(currentPlotIndex + 1);
        return false;
    }
}

bool farm::willItConflict(int currentPlotIndex, riceVariety *possibleVariety)
{
    plot *currentPlot = farmPlotList[currentPlotIndex];
    for (auto &neighbouringPlot : farmPlotAdjacencyMap[currentPlot])
    {
        riceVariety *varietyOnNeighbour = neighbouringPlot->assignedVariety;
        if (varietyOnNeighbour == NULL)
        {
            continue;
        }

        if ((possibleVariety->nonConflictingNeighbours).find(varietyOnNeighbour) == (possibleVariety->nonConflictingNeighbours).end())
        {
            return true;
        }
    }
    return false;
}

void farm::placeRiceinGrid()
{
    try
    {
        populateRiceVarietyList();

        if (rows*columns < (int)riceVarietyList.size())
        {
            string errorStr("ERROR : Total number of plots ");
            errorStr += farmPlotAdjacencyMap.size();
            errorStr += " is less than the number of varieties ";
            errorStr += riceVarietyList.size();
            throw errorStr;
        }

        populateNonConflictingNeighbours();

        //sortListAsPerDesiredOrder();

        bool retValue = recurseAndAssignVarietyToPlots(0);

        if (retValue == true)
        {
            decompileFarmGridInCSV();
        }
        else
        {
            throw "ERROR : could not find a possible solution for given list of variety, please try by reducing the list of varieties";
        }
    }
    catch (string &errorStr)
    {
        cout << errorStr << endl;
    }
    catch (char *errorStr)
    {
        cout << errorStr << endl;
    }
}

void farm::decompileFarmGridInCSV()
{
    ofstream outfile;
    outfile.open(outputFilePath);
    if (!outfile)
    {
        throw "ERROR: Input file '" + outputFilePath + "' could not be opened, check if it is present";
    }

    for (int hindex=0; hindex<rows; ++hindex)
    {
        for (int vindex=0; vindex<columns; ++vindex)
        {
            if (farmGrid[hindex][vindex]->assignedVariety)
            {
                string vName = farmGrid[hindex][vindex]->assignedVariety->name;
                Date dt = farmGrid[hindex][vindex]->assignedVariety->sDate;
                outfile << vName.c_str() << ","; //" " << dt.day << "/" << dt.month << "," ;
            }
            else
            {
                outfile << "  ";
            }
        }
        outfile << endl;
    }
}

