#include "../B1/billing.hpp"
#include <functional>
#include <cassert>
int main()
{
    double limit = 100000, copay = 0.2;
    std::function<double(const Bill&)> policy = [limit, copay](const Bill& b) {
        double claim = b.total() * (1 - copay);
        if (claim > limit) throw ClaimError("claim above cover");
        return claim;
    };
    OutPatientBill b("Ravi", 500, 1200);
    assert(policy(b) == 1360);
    std::cout << "Policy covered=" << policy(b)
              << " sizeof(std::function)=" << sizeof(policy)
              << " sizeof(Insurance)=" << sizeof(Insurance) << '\n';
}
