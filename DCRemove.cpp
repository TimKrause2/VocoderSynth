#include "DCRemove.h"

#define ALPHA_DC 0.997

DCRemove::DCRemove(void)
{
    z1 = 0.0f;
}

float DCRemove::Evaluate(float x)
{
    float m = x + ALPHA_DC*z1;
    float r = m - z1;
    z1 = m;
    return r;
}
