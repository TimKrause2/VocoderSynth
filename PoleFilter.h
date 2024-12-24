#include <memory>

struct PoleFilter
{
    std::unique_ptr<double[]> z;
    std::unique_ptr<double[]> a;
    int i_z;
    int order;
    PoleFilter(int order);
    double* CoeffPtr(void);
    float Evaluate(float x);
};
