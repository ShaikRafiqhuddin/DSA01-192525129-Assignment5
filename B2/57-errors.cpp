#include "../B1/billing.hpp"
#include <cassert>
#include <limits>
#include <new>
struct Guard {
    static int alive;
    Guard() { ++alive; std::cout << "acquire "; }
    ~Guard() noexcept { --alive; std::cout << "release "; }
};
int Guard::alive = 0;
void exercise(int test)
{
    Guard guard;
    switch (test) {
    case 0: {
        OutPatientBill b("P1", -1, 20);
        break;
    }
    case 1: {
        DayCareBill b("P1", 60000);
        Insurance payer(50000, 0);
        payer.amountCovered(b);
        break;
    }
    case 2: {
        DayCareBill b("P1", 20000);
        GovernmentScheme payer({{"OP", 100}});
        payer.amountCovered(b);
        break;
    }
    case 3:
        std::stod("not-a-number");
        break;
    case 4: {
        volatile std::size_t impossible = std::numeric_limits<std::size_t>::max();
        void* memory = ::operator new(impossible);
        ::operator delete(memory);
        break;
    }
    case 5: {
        OutPatientBill b("", 1, 2);
        break;
    }
    case 6:
        throw 42;
    }
}
int main()
{
    for (int test = 0; test < 7; ++test) {
        std::cout << "test " << test << ": ";
        try {
            exercise(test);
        } catch (const std::bad_alloc&) {
            std::cout << "caught bad_alloc";
        } catch (const std::exception& e) {
            std::cout << "caught " << e.what();
        } catch (...) {
            std::cout << "caught unknown exception";
        }
        assert(Guard::alive == 0);
        std::cout << "; alive=" << Guard::alive << '\n';
    }
    std::cout << "PASS: continued after all 7 errors; no live guards\n";
}
