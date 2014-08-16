#pragma once

#include "stdafx.h"

class Date
{
public:
    int                 day;
    int                 month;
    int                 year;

    Date(string &str)
    {
        year = 9999; // unused so marking as random

        size_t dayEnd = str.find_first_of('/');
        if (dayEnd != string::npos)
        {
            day = atoi((str.substr(0, dayEnd)).c_str());
        }
        else
        {
            throw "ERROR: input date is '" + str + "' is not in correct format. Use dd/mm format ";
        }

        //size_t monthEnd = str.find_first_of('/', dayEnd + 1);
        //if (monthEnd != string::npos)
        //{
        month = atoi((str.substr(dayEnd+1)).c_str());
        //}
        //else
        //{
        //    throw "ERROR: input date is '" + str + "' is not in correct format. Use dd/mm format ";
        //}

        if (day < 1 || day > 31 || month < 1 || month > 12)
        {
            throw "ERROR: input date is '" + str + "' is not in correct range" ;
        }
    }

    bool operator<=(const Date &rhs)
    {
        if (this->month == rhs.month)
        {
            if (this->day <= rhs.day)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (this->month < rhs.month)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    //friend ofstream& operator<<(ofstream &output, const Date &d);
};

//ofstream& operator<<(ofstream &output, const Date &d)
//{
//    output << d.day << "/" << d.month;
//    return output;
//}

class riceVariety
{
public:
    string              name;
    Date                sDate;
    Date                eDate;
    set<riceVariety *>  nonConflictingNeighbours;
    bool                used;

    riceVariety(string &varname, string &sd, string &ed) : name(varname), used(false), sDate(sd), eDate(ed)
    {}

    ~riceVariety()
    {}

    bool pollinationConflicts(riceVariety *rhs)
    {
        if (this->sDate <= rhs->sDate)
        {
            if (rhs->sDate <= this->eDate)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (this->sDate <= rhs->eDate)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
};