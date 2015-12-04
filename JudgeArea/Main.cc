#include <iostream>
#include <stdio.h>
#include <string>
#include <assert.h>
 
using namespace std;
 
int gcd(int a, int b) {
    int x=a, y=b, z=x%y;
    while(z) x=y, y=z, z=x%y;
    return y;
}
 
int main() {
    int t, a, b;
    cin>>t;
    assert(t>=1);
    while(t--) {
        cin>>a>>b;
        assert(0<a && a<=1000000);
        assert(0<b && b<=1000000);
        assert(gcd(a, 26)==1);
        string str;
        cin>>str;
        assert(1<=str.size() && str.size()<=1000000);
        for(int i=0 ; i<str.size() ; ++i) {
            int idx=str[i]-'A';
            assert(0<=idx && idx<=25);
            int sol=(idx*a+b)%26;
            cout<<(char)(sol+'A');
        }
        cout<<endl;
    }
}