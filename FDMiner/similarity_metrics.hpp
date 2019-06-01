#include <cstdlib>
#include <vector>
#include <iostream>
#include <regex>

struct Date
{
    int d, m, y;
};

struct Time
{
    unsigned int h, m;
};

// This function counts number of leap years before the given date
int countLeapYears(Date d)
{
    int years = d.y;

    // Check if the current year needs to be considered needs to be considered
    // for the count of leap years or not
    if(d.m <= 2)
    {
        years--;
    }

    // A year is a leap year if it is a multiple of 4, multiple of 400 and not
    // a multiple of 100
    return years / 4 - years / 100 + years / 400;
}

// This function returns the number of days between two given dates
long getDifference(const Date& dt1, const Date& dt2)
{
    // To store the number of days in each month
    const int monthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // Count total number of days before first date

    // Initialize count using day and years
    long int n1 = dt1.y * 365 + dt1.d;

    // Add days form months in given date
    for(int i = 0; i < dt1.m - 1; i++)
    {
        n1 += monthDays[i];
    }

    // Since very leap year is off 366 days. Add a day for eavery leapp day
    n1 += countLeapYears(dt1);

    // Similary, count total number of days before dt2
    long int n2 = dt2.y * 365 + dt2.d;

    for(int i = 0; i < dt2.m - 1; i++)
    {
        n2 += monthDays[i];
    }

    n2 += countLeapYears(dt2);

    // return difference between two counts
    return abs(n2 - n1);
}

long getDifference(const Time& t1, const Time& t2)
{
    long m1 = t1.h * 60 + t1.m;
    long m2 = t2.h * 60 + t2.m;

    return abs(m1 - m2);
}

float getDifference(const std::vector<float>& x, const std::vector<float>& y)
{
    float sum = 0.0f;

    for(int i = 0; i < x.size(); i++)
    {
        sum += ((x[i] - y[i]) * (x[i] - y[i]));
    }

    return sqrtf(sum);
}

size_t getDifference(const std::string &s1, const std::string &s2)
{
    const size_t m(s1.size());
    const size_t n(s2.size());

    if( m==0 ) return n;
    if( n==0 ) return m;

    size_t *costs = new size_t[n + 1];

    for( size_t k=0; k<=n; k++ ) costs[k] = k;

    size_t i = 0;
    for ( std::string::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i )
    {
        costs[0] = i+1;
        size_t corner = i;

        size_t j = 0;
        for ( std::string::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j )
        {
            size_t upper = costs[j+1];
            if( *it1 == *it2 )
            {
                costs[j+1] = corner;
            }
            else
            {
                size_t t(upper<corner?upper:corner);
                costs[j+1] = (costs[j]<t?costs[j]:t)+1;
            }

            corner = upper;
        }
    }

    size_t result = costs[n];
    delete [] costs;

    return result;
}
