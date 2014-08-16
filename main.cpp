#include "stdafx.h"
#include "Farm.h"

int _tmain(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "ERROR: incorrect number of arguments provided as input. Check README file for more help" << endl;
        return 1;
    }
    
    int rows = atoi(argv[1]);
    int columns = atoi(argv[2]);
    
    farm *riceFarm = new farm(rows, columns);
        
    riceFarm->placeRiceinGrid();

    delete riceFarm;
    return 0;
}
