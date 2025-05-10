#include <iostream>
#include "MyStruct.h"

using namespace std;
int main() 
{
    MyStruct a{1, 2, 3, 4, 5};
    for (int i = 0; i < 5; ++i) {
        a.push_back(i+10);
        a.push_front(i+20);
    }
    for (auto it = a.begin(); it != a.end(); ++it) {
        cout << *it << " ";
    }
    auto x = a.erase(--a.cend());
    cout << endl;
    cout << ( x == a.end() ) << endl;
    for (auto it = a.begin(); it != a.end(); ++it) {
        cout << *it << " ";
    }
    return 0;
}
