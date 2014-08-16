#pragma once

#include "RiceVariety.h"

class plot
{
public:
    plot(int i=0, int j=0, int count =-1) : rowIndex(i), colIndex(j), plotNameAsIndex(count), assignedVariety(NULL) {}

    int                                 rowIndex;
    int                                 colIndex;
    int                                 plotNameAsIndex;
    riceVariety                        *assignedVariety;
};

class farm
{
public:
    farm(int i = 0, int j = 0, char *inputFilePath = NULL, char *outputFilePath = NULL);
    ~farm();

    void                                placeRiceinGrid();

private:
    map<plot *, vector<plot *> >        farmPlotAdjacencyMap;
    plot                             ***farmGrid;
    vector<plot *>                      farmPlotList;
    vector<riceVariety *>               riceVarietyList;
    int                                 rows;
    int                                 columns;
    int                                 maxOccupiedPlots;
    string                              inputFilePath;
    string                              outputFilePath;

    void                                populateRiceVarietyList();
    void                                populateNonConflictingNeighbours();
    void                                sortListAsPerDesiredOrder();
    bool                                recurseAndAssignVarietyToPlots(int currentPlotIndex);
    bool                                willItConflict(int currentPlotIndex, riceVariety *possibleVariety);
    void                                decompileFarmGridInCSV();
};