#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>



/**
 * Library-level functions.
 * You should use them in the main sections.
 */

uint64_t convertToUint64(double number) {
    return *((uint64_t*)(&number));
}

bool getBit(const uint64_t number, const uint8_t index) {
    uint64_t mask = 1ll << index;
    uint64_t temp = mask & number;
    temp >>= index;
    return temp;
}

bool isExponentZero(const uint64_t number)
{
    for (int i = 62; i >= 52; i--)
    {
        if (getBit(number, i)) { return false; }
    }
    return true;
}

bool isExponentAllOnes(const uint64_t number)
{
    for (int i = 62; i >= 52; i--)
    {
        if (!getBit(number, i)) { return false; }
    }
    return true;
}

bool isFractionZero(const uint64_t number)
{
    for (int i = 51; i >= 0; i--)
    {
        if (getBit(number, i)) { return false; }
    }
    return true;
}

bool getSign(const uint64_t number)
{
    return getBit(number, 63);
}

bool isDenormalized(const uint64_t number)
{
    return isExponentZero(number) && getBit(number, 0);
}

bool isNormalized(const uint64_t number)
{
    return !isExponentZero(number) && !isExponentAllOnes(number);
}

bool isInfinity(const uint64_t number)
{
    return isExponentAllOnes(number) && isFractionZero(number);
}

/**
 * Checkers here:
 */

bool checkForPlusZero(uint64_t number) {
    return number == 0ll;
}

bool checkForMinusZero(uint64_t number) {
    return number == 0x8000000000000000;
}

bool checkForPlusInf(uint64_t number) {
    return isInfinity(number) && !getSign(number);
}

bool checkForMinusInf(uint64_t number) {
    return isInfinity(number) && getSign(number);
}

bool checkForPlusNormal(uint64_t number) {
    return isNormalized(number) && !getSign(number);
}

bool checkForMinusNormal(uint64_t number) {
    return isNormalized(number) && getSign(number);
}

bool checkForPlusDenormal(uint64_t number) {
    return isDenormalized(number) && !getSign(number);
}

bool checkForMinusDenormal(uint64_t number) {
    return isDenormalized(number) && getSign(number);
}

bool checkForSignalingNan(uint64_t number) {
    return getSign(number) && isExponentAllOnes(number) && !getBit(number,51) && getBit(number, 0);
}

bool checkForQuietNan(uint64_t number) {
    return getSign(number) && isExponentAllOnes(number) && getBit(number, 51);
}


void classify(double number) {
    if (checkForPlusZero(convertToUint64(number))) {
        printf("Plus zero\n");
    }

    else if (checkForMinusZero(convertToUint64(number))) {
        printf("Minus zero\n");
    }

    else if (checkForPlusInf(convertToUint64(number))) {
        printf("Plus inf\n");
    }

    else if (checkForMinusInf(convertToUint64(number))) {
        printf("Minus inf\n");
    }

    else if (checkForPlusNormal(convertToUint64(number))) {
        printf("Plus normal\n");
    }

    else if (checkForMinusNormal(convertToUint64(number))) {
        printf("Minus normal\n");
    }

    else if (checkForPlusDenormal(convertToUint64(number))) {
        printf("Plus Denormal\n");
    }

    else if (checkForMinusDenormal(convertToUint64(number))) {
        printf("Minus Denormal\n");
    }

    else if (checkForSignalingNan(convertToUint64(number))) {
        printf("Signailing NaN\n");
    }

    else if (checkForQuietNan(convertToUint64(number))) {
        printf("Quiet NaN\n");
    }

    else {
        printf("Error.\n");
    }
}