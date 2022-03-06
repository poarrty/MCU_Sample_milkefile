/* CPP:SIGNAL try{}catch{} not supports */
extern "C" {
#include "test.h"
#define LOG_TAG "test_cpp"
#include "elog.h"
}

#include <algorithm>   //STL 通用算法
#include <bitset>      //STL 位集容器
#include <cctype>      //字符处理
#include <cerrno>      //定义错误码
#include <cfloat>      //浮点数处理
#include <ciso646>     //对应各种运算符的宏
#include <climits>     //定义各种数据类型最值的常量
#include <clocale>     //定义本地化函数
#include <cmath>       //定义数学函数
#include <complex>     //复数类
#include <csetjmp>     //异常处理支持
#include <csignal>     //信号机制支持
#include <cstdarg>     //不定参数列表支持
#include <cstddef>     //常用常量
#include <cstdio>      //定义输入／输出函数
#include <cstdlib>     //定义杂项函数及内存分配函数
#include <cstring>     //字符串处理
#include <ctime>       //定义关于时间的函数
#include <cwchar>      //宽字符处理及输入／输出
#include <cwctype>     //宽字符分类
#include <deque>       //STL 双端队列容器
#include <exception>   //异常处理类
#include <fstream>     //文件输入／输出
#include <functional>  //STL 定义运算函数（代替运算符）
#include <iomanip>     //参数化输入／输出
#include <ios>         //基本输入／输出支持
#include <iosfwd>      //输入／输出系统使用的前置声明
#include <iostream>    //数据流输入／输出
#include <istream>     //基本输入流
#include <iterator>    //STL迭代器
#include <limits>      //定义各种数据类型最值常量
#include <list>        //STL 线性列表容器
#include <locale>      //本地化特定信息
#include <map>         //STL 映射容器
#include <memory>      //STL通过分配器进行的内存分配
#include <new>         //动态内存分配
#include <numeric>     //STL常用的数字操作
#include <ostream>     //基本输出流
#include <queue>       //STL 队列容器
#include <set>         //STL 集合容器
#include <sstream>     //基于字符串的流
#include <stack>       //STL 堆栈容器
#include <stdexcept>   //标准异常类
#include <streambuf>   //底层输入／输出支持
#include <string>      //字符串类
#include <typeinfo>    //运行期间类型信息
#include <utility>     //STL 通用模板类
#include <valarray>    //对包含值的数组的操作
#include <vector>      //STL 动态数组容器

#include <complex.h>   //复数处理
#include <fenv.h>      //浮点环境
#include <inttypes.h>  //整数格式转换
#include <stdbool.h>   //布尔环境
#include <stdint.h>    //整型环境
#include <tgmath.h>    //通用类型数学宏

#include <cstdlib>
#include <pthread.h>

using namespace std;

#define NUM_THREADS 5

void   cpp_main(void);
double division(int a, int b);
template <typename T>
void sort(T* arr,int s,int e);

int test(void) {
    std::cout << "Hello, world!" << std::endl;
    log_e("cccccccccccccccccccccccccc");
    cpp_main();
    return 0;
}

template <typename T>
inline T const &Max(T const &a, T const &b) {
    return a < b ? b : a;
}

void cpp_main(void) {
    // int i = 39;
    // int j = 20;
    // cout << "Max(i, j): " << Max(i, j) << endl; 
 
    // double f1 = 13.5; 
    // double f2 = 20.7; 
    // cout << "Max(f1, f2): " << Max(f1, f2) << endl; 
 
    // string s1 = "Hello"; 
    // string s2 = "World"; 
    // cout << "Max(s1, s2): " << Max(s1, s2) << endl; 

    //vector<float> fvec = {0.8,0.9,0.5,0.4,-0.3};
    vector<int> vec = {34,-56,0,-67,44,16,87,-98,54,43,32,17,56,56,88,-1,0,1,2,3,4,5,6,7,8,9,10,33,56,78,66,78,90,1099,4678,898};
   
 
   // 显示 vec 的原始大小
   cout << "vector size = " << vec.size() << endl;

   sort<int>(&vec[0],0,vec.size()-1);
   //sort<float>(&fvec[0],0,fvec.size()-1);
 
    // 使用迭代器 iterator 访问值
    // vector<float>::iterator fv = fvec.begin();
    // while( fv != fvec.end()) {
    //     cout << "value of v = " << *fv << endl;
    //     fv++;
    // }

    vector<int>::iterator v = vec.begin();
    while( v != vec.end()) {
        cout << "value of v = " << *v << endl;
        v++;
    }
}

extern "C" {
void test_cpp(void) {
    test();
}
}

template <typename T>
void sort(T* arr,int s,int e){
    T* pi = &arr[s];
    T* pj = &arr[e];
    T k = *pi;
    int ki;
    if(s<e){
        while(pi<pj){
            while(pi<pj && k<=*pj)
                pj--;
            if(pi<pj) {
                *pi = *pj;
                *pj = k;
            }
            while(pi<pj && k>=*pi)
                pi++;
            if(pi<pj){
                *pj=*pi;
                *pi=k;
            }
        }
        ki = pi - &arr[s] + s;
        
        sort(arr,s,ki-1);
        sort(arr,ki+1,e);
    }
}
