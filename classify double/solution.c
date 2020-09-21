#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Library-level functions.
 * You should use them in the main sections.
 */

uint64_t convertToUint64(double number) 
{
    return *((uint64_t*)(&number));
}

bool getBit(const uint64_t number, const uint8_t index) 
{
    return (number & (1ll << index)) != 0;
}

bool isExponentZero(const uint64_t number)
{
    uint64_t tmp = number >> 52;
    return tmp == 0 || tmp == 2048;
}

bool isExponentAllOnes(const uint64_t number)
{
    uint64_t tmp = number >> 52;
    return tmp == 2047 || tmp == 4095;
}

bool isFractionZero(const uint64_t number)
{
    return ((number << 12) >> 12) == 0;
}

bool isFractionAllOnes(const uint64_t number)
{
    return ((number << 12) >> 12) == 4503599627370495;
}

bool getSign(const uint64_t number)
{
    return getBit(number, 63);
}

bool isDenormalized(const uint64_t number)
{
    return isExponentZero(number) && !isFractionZero(number, 0);
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
    return isExponentAllOnes(number) && !isFractionZero(number) && !isFractionAllOnes(number);
}

bool checkForQuietNan(uint64_t number) {
    return isExponentAllOnes(number) && getBit(number, 51);
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