/*
 * @file    rsa.c
 * @author  이하영 / 2017012479
 * @date    작성 일자
 * @brief   mini RSA implementation code
 * @details 세부 설명
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "rsa.h"

llint p, q, e, d, n;

// #define getQuotient(x, n) (x >> (n >> 1))
#define getQuotient(x) (x >> 32)
#define getRemainder(x) (x & 0xffffffff)
#define getDividedbyTwo(x) (x >> 1)
#define isODD(x) ((x & 0x01) == 0x01)

llint division(llint x, llint n){
    llint remainder = x;
    llint quotient = 0;
    llint result;

    if(remainder < 0){
        while(remainder >= 0)
            remainder += n;
    }

    while(remainder >= n){
        remainder -= n;
        quotient++;
    }

    result = quotient;
    result = result << 32;
    result += remainder;

    return result;
}

/*
 * @brief     모듈러 덧셈 연산을 하는 함수.
 * @param     llint a     : 피연산자1.
 * @param     llint b     : 피연산자2.
 * @param     byte op    : +, - 연산자.
 * @param     llint n      : 모듈러 값.
 * @return    llint result : 피연산자의 덧셈에 대한 모듈러 연산 값. (a op b) mod n
 * @todo      모듈러 값과 오버플로우 상황을 고려하여 작성한다.
 */
llint ModAdd(llint a, llint b, byte op, llint n) {
    llint result;
    llint x, y;

    x = getRemainder(division(a, n));
    y = getRemainder(division(b, n));

    if(x == 0)
        return y;
    if(y == 0)
        return x;
    
    if(x + y <= x)
        result = getRemainder(division(x - (n - y), n));
    else
        result = getRemainder(division(x + y, n));

    return result;
}


/*
 * @brief      모듈러 곱셈 연산을 하는 함수.
 * @param      llint x       : 피연산자1.
 * @param      llint y       : 피연산자2.
 * @param      llint n       : 모듈러 값.
 * @return     llint result  : 피연산자의 곱셈에 대한 모듈러 연산 값. (a x b) mod n
 * @todo       모듈러 값과 오버플로우 상황을 고려하여 작성한다.
 */
llint ModMul(llint x, llint y, llint n){
    llint result = 0;
    llint a = x, b = y;

    a = getRemainder(division(a, n));
    // b = getRemainder(b, n);

    if(a == 0 || b == 0)
        return 0;
    else if(a == 1)
        return getRemainder(division(b, n));
    else if(b == 1)
        return a;

    while(b > 0){
        if(isODD(b)){
            result = ModAdd(result, a, '+', n);
        }
        // a = ModAdd(a, a, '+', n);
        a = getRemainder(division((a << 1), n));
        b = getDividedbyTwo(b);
    }

    return result;
}

/*
 * @brief      모듈러 거듭제곱 연산을 하는 함수.
 * @param      llint base   : 피연산자1.
 * @param      llint exp    : 피연산자2.
 * @param      llint n      : 모듈러 값.
 * @return     llint result : 피연산자의 연산에 대한 모듈러 연산 값. (base ^ exp) mod n
 * @todo       모듈러 값과 오버플로우 상황을 고려하여 작성한다.
               'square and multiply' 알고리즘을 사용하여 작성한다.
 */
llint ModPow(llint base, llint exp, llint n) {
    llint result = 1, tmp;
    llint x = base, y = exp;

    if((x < 1) || (n < 1))
        return 0;
    if(y == 0)
        return 1;

    // x = getRemainder(x, n);

    if(y == 1)
        return getRemainder(division(x, n));

    while(y > 0){
        if(isODD(y)){
            result = ModMul(result, x, n);
        }
        x = ModMul(x, x, n);
        y = getDividedbyTwo(y);
    }
    
    return result;
}

/*
 * @brief      입력된 수가 소수인지 입력된 횟수만큼 반복하여 검증하는 함수.
 * @param      llint testNum   : 임의 생성된 홀수.
 * @param      llint repeat    : 판단함수의 반복횟수.
 * @return     bool isPrime    : 판단 결과에 따른 TRUE, FALSE 값.
 * @todo       Miller-Rabin 소수 판별법과 같은 확률적인 방법을 사용하여,
               이론적으로 4N(99.99%) 이상 되는 값을 선택하도록 한다. 
 */
bool IsPrime(llint testNum, llint repeat) {
    llint n = testNum;
    llint s = n - 1;
    llint a, tmp;

    a = (llint) (WELLRNG512a() * (s - 1)) + 1; // random integer

    while (!isODD(s)) {
        if (ModPow(a, s, n) == n - 1)
            return TRUE;
        s = getDividedbyTwo(s);
    }

    tmp = ModPow(a, s, n);

    return tmp == n - 1 || tmp == 1;
}

/*
 * @brief       모듈러 역 값을 계산하는 함수.
 * @param       llint a      : 피연산자1.
 * @param       llint m      : 모듈러 값.
 * @return      llint result : 피연산자의 모듈러 역수 값.
 * @todo        확장 유클리드 알고리즘을 사용하여 작성하도록 한다.
 */
/*
llint ModInv(llint a, llint m) {
    llint result = 0;
    llint q, r;
    llint s = 0, x0=1, x1=0;
    llint t = 0, y0=0, y1=1;

    while(m > 0)
    {
        q = getQuotient(division(a, m));
        r = getRemainder(division(a, m));

        // printf("a is %llu, m is %llu\n", a, m);
        // printf("quotient : %llu, remainder : %llu\n", quotient, remainder);

        s = x0 - (q * x1);
        t = y0 - (q * y1);

        x0 = x1; 
        x1 = s;
        y0 = y1;
        y1 = t;

        a = m;
        m = r;
    }
    if(a == 1) 
        result = y0;

    return result;
}
*/

llint ModInv(llint a, llint m) {
    llint result;
    llint t0 = 0, t1 = 1, s0 = 0, s1 = 1, r0 = a, r1 = m;
    llint q, r, tmp;

    while(r1 != 0){
        q = getQuotient(division(r0, r1));
        // r = getRemainder(division(r0, r1));

        tmp = t1;
        t1 = t0 - q * t1;
        t0 = tmp;

        tmp = r1;
        r1 = r0 - q * r1;
        r0 = tmp;
    }

    if(r0 > 1){ // not invertible
        printf("not invertible\n");
        exit(1);
    }
    if(t0 < 0)
        t0 += n;
    
    result = t0;

    return result;
}

/*
 * @brief     RSA 키를 생성하는 함수.
 * @param     llint *p   : 소수 p.
 * @param     llint *q   : 소수 q.
 * @param     llint *e   : 공개키 값.
 * @param     llint *d   : 개인키 값.
 * @param     llint *n   : 모듈러 n 값.
 * @return    void
 * @todo      과제 안내 문서의 제한사항을 참고하여 작성한다.
 */
void miniRSAKeygen(llint *p, llint *q, llint *e, llint *d, llint *n) {
    llint phi_n;

    do{
        *p = getRemainder(division(WELLRNG512a() * 100000000000000000, (RND_MAX-RND_MIN)) + RND_MIN); // random prime
    } while(IsPrime(*p, 5));

    do{
        *q = getRemainder(division(WELLRNG512a() * 100000000000000000, (RND_MAX-RND_MIN)) + RND_MIN); // random prime
    } while(IsPrime(*q, 5));

    // *p = 3;
    // *q = 7;
    // *e = 5;

    *n = *p * *q;
    phi_n = (*p - 1) * (*q - 1);

    // e : <= phi_n and relatively prime with phi_n
    // compute d : e mod phi_n, ed mod phi_n = 1
    do{
        *e = (llint) (WELLRNG512a() * 100000000000000000);
        *d = ModInv(phi_n, *e);
        printf("phi_n is %llu, e is %llu, d is %llu\n", phi_n, *e, *d);
    } while(GCD(*e, phi_n) != 1 || phi_n <= *e || ModMul(*e, *d, phi_n) != 1);
}

/*
 * @brief     RSA 암복호화를 진행하는 함수.
 * @param     llint data   : 데이터 값.
 * @param     llint key    : 키 값.
 * @param     llint n      : 모듈러 n 값.
 * @return    llint result : 암복호화에 결과값
 * @todo      과제 안내 문서의 제한사항을 참고하여 작성한다.
 */
llint miniRSA(llint data, llint key, llint n) {
    llint result;

    printf("data : %llu, key : %llu, n : %llu\n", data, key, n);
    result = ModPow(data, key, n);
    printf("data : %llu, key : %llu, n : %llu\n", data, key, n);

    return result;
}

llint GCD(llint a, llint b) {
    llint prev_a;

    while(b != 0) {
        // printf("GCD(%llu, %llu)\n", a, b);
        prev_a = a;
        a = b;
        while(prev_a >= b) prev_a -= b;
        b = prev_a;
    }
    printf("GCD(%llu, %llu)\n\n", a, b);
    return a;
}

int main(int argc, char* argv[]) {
    byte plain_text[4] = {0x12, 0x34, 0x56, 0x78};
    llint plain_data, encrpyted_data, decrpyted_data;
    uint seed = time(NULL);

    memcpy(&plain_data, plain_text, 4);

    // 난수 생성기 시드값 설정
    seed = time(NULL);
    InitWELLRNG512a(&seed);

    printf("22 + 31 mod 12 is 5, %llu\n", ModAdd(22, 31, '+', 12));
    printf("8^2 mod 3 is 1, %llu\n", ModPow(8, 2, 3));
    printf("11 * 2 mod 9 is 4, %llu\n", ModMul(11, 2, 9));
    printf("7*x mod 160 is 23, %llu\n\n", ModInv(160, 7));

    // RSA 키 생성
    miniRSAKeygen(&p, &q, &e, &d, &n);
    printf("0. Key generation is Success!\n ");
    printf("p : %llu\n q : %llu\n e : %llu\n d : %llu\n n : %llu\n\n", p, q, e, d, n);

    // RSA 암호화 테스트
    encrpyted_data = miniRSA(plain_data, e, n);
    printf("1. plain text : %llu\n", plain_data);    
    printf("2. encrypted plain text : %llu\n\n", encrpyted_data);

    // RSA 복호화 테스트
    decrpyted_data = miniRSA(encrpyted_data, d, n);
    printf("3. cipher text : %llu\n", encrpyted_data);
    printf("4. Decrypted plain text : %llu\n\n", decrpyted_data);

    // 결과 출력
    printf("RSA Decryption: %s\n", (decrpyted_data == plain_data) ? "SUCCESS!" : "FAILURE!");

    return 0;
}