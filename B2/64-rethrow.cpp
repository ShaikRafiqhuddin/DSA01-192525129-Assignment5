#include <iostream>
#include <stdexcept>
struct ClaimError : std::runtime_error {
    using std::runtime_error::runtime_error;
};
struct SpecificClaim : ClaimError {
    using ClaimError::ClaimError;
};
void settle(bool copy)
{
    try {
        throw SpecificClaim("special claim");
    } catch (const ClaimError& e) {
        if (copy) throw e;
        throw;
    }
}
int main()
{
    for (bool copy : {false, true}) {
        std::cout << (copy ? "throw e: " : "throw;: ");
        try {
            settle(copy);
        } catch (const SpecificClaim&) {
            std::cout << "SpecificClaim preserved\n";
        } catch (const ClaimError&) {
            std::cout << "ClaimError only (sliced)\n";
        }
    }
}
