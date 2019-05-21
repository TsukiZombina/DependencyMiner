#include <iostream>
#include <regex>

size_t uiLevenshteinDistance(const std::string &s1, const std::string &s2)
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

unsigned int getTimeSpan(const std::string& dateStr1, const std::string& dateStr2)
{
    // Parse strings
    std::regex re("[/]+");
    std::sregex_token_iterator it1(dateStr1.begin(), dateStr1.end(), re, -1);
    std::sregex_token_iterator it2(dateStr2.begin(), dateStr2.end(), re, -1);
    std::sregex_token_iterator reg_end;

    int date1[3] = {0, 0, 0};
    int date2[3] = {0, 0, 0};

    for (int i = 0; it1 != reg_end && it2 != reg_end; ++it1, ++it2, ++i) {
        date1[i] = std::stoi(it1->str());
        date2[i] = std::stoi(it2->str());
    }

    return 0;
}
