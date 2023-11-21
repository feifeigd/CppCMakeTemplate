
#include <concepts>
#include <iostream>

using namespace std;

// 使用 concept 的四个变种

template<typename T>
    requires integral<T>
auto gcd(T a, T b){
    if(0 == b)
        return a;
    return gcd(b, a % b);
}

template<typename T>    
auto gcd1(T a, T b)requires integral<T>{
    if(0 == b)
        return a;
    return gcd1(b, a % b);
}

template<integral T>    
auto gcd2(T a, T b){
    if(0 == b)
        return a;
    return gcd2(b, a % b);
}

auto gcd3(integral auto a, integral auto b){
    if(0 == b)
        return a;
    return gcd3(b, a % b);
}

int main(){
    cout << "\n";
    cout << "gcd(100, 10)= " << gcd(100, 10) << "\n";
    cout << "gcd1(100, 10)= " << gcd1(100, 10) << "\n";
    cout << "gcd2(100, 10)= " << gcd2(100, 10) << "\n";
    cout << "gcd3(100, 10)= " << gcd3(100, 10) << "\n";
    cout << endl;
    return 0;
}
