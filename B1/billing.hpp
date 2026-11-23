#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class BillingError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
class InvalidCharge : public BillingError {
public:
    using BillingError::BillingError;
};
class MissingPatient : public BillingError {
public:
    using BillingError::BillingError;
};
class ClaimError : public BillingError {
public:
    using BillingError::BillingError;
};
class UnknownPackage : public ClaimError {
public:
    using ClaimError::ClaimError;
};
inline double checked(double x)
{
    if (!std::isfinite(x) || x < 0) {
        throw InvalidCharge("charge must be finite and nonnegative");
    }
    return x;
}
class Bill {
    std::string patientId;
public:
    explicit Bill(std::string p) : patientId(std::move(p))
    {
        if (patientId.empty()) throw MissingPatient("missing patient record");
    }
    virtual ~Bill() noexcept = default;
    virtual double total() const = 0;
    virtual std::string type() const = 0;
    virtual std::string packageCode() const { return type(); }
    const std::string& patient() const noexcept { return patientId; }
    void print(std::ostream& out) const
    {
        out << type() << ' ' << patient() << ": " << total() << '\n';
    }
};
class OutPatientBill : public Bill {
    double consultation, tests;
public:
    OutPatientBill(std::string p, double c, double t)
        : Bill(std::move(p)), consultation(checked(c)), tests(checked(t)) {}
    double total() const override { return checked(consultation + tests); }
    std::string type() const override { return "OP"; }
};
class InPatientBill : public Bill {
    int days;
    double perDay, procedures, medicines;
public:
    InPatientBill(std::string p, int d, double r, double s, double m)
        : Bill(std::move(p)), days(d), perDay(checked(r)),
          procedures(checked(s)), medicines(checked(m))
    {
        if (d < 0) throw InvalidCharge("negative days");
    }
    double total() const override
    {
        return checked(days * perDay + procedures + medicines);
    }
    std::string type() const override { return "IP"; }
};
class DayCareBill : public Bill {
    double price;
public:
    DayCareBill(std::string p, double amount)
        : Bill(std::move(p)), price(checked(amount)) {}
    double total() const override { return price; }
    std::string type() const override { return "DC"; }
};
class EmergencyBill : public Bill {
    double base, hours, hourly;
public:
    EmergencyBill(std::string p, double b, double h, double r)
        : Bill(std::move(p)), base(checked(b)), hours(checked(h)), hourly(checked(r)) {}
    double total() const override { return checked(base + hours * hourly); }
    std::string type() const override { return "ER"; }
};
class Payer {
public:
    virtual ~Payer() noexcept = default;
    virtual double amountCovered(const Bill& b) const = 0;
    virtual std::string name() const = 0;
};
class SelfPay : public Payer {
public:
    double amountCovered(const Bill&) const override { return 0; }
    std::string name() const override { return "SelfPay"; }
};
class Insurance : public Payer {
    double limit, copay;
public:
    Insurance(double l, double c) : limit(checked(l)), copay(checked(c))
    {
        if (c > 1) throw ClaimError("copay outside [0,1]");
    }
    double amountCovered(const Bill& b) const override
    {
        double claim = checked(b.total() * (1 - copay));
        if (claim > limit) throw ClaimError("claim above cover");
        return claim;
    }
    std::string name() const override { return "Insurance"; }
};
class GovernmentScheme : public Payer {
    std::map<std::string, double> rates;
public:
    explicit GovernmentScheme(std::map<std::string, double> r) : rates(std::move(r))
    {
        for (const auto& entry : rates) checked(entry.second);
    }
    double amountCovered(const Bill& b) const override
    {
        auto it = rates.find(b.packageCode());
        if (it == rates.end()) throw UnknownPackage("unknown package " + b.packageCode());
        return std::min(b.total(), it->second);
    }
    std::string name() const override { return "Government"; }
};
inline std::vector<std::unique_ptr<Bill>> standardBills()
{
    std::vector<std::unique_ptr<Bill>> bills;
    bills.push_back(std::make_unique<OutPatientBill>("Ravi", 500, 1200));
    bills.push_back(std::make_unique<InPatientBill>("Meena", 3, 2500, 40000, 1500));
    bills.push_back(std::make_unique<DayCareBill>("Anu", 20000));
    bills.push_back(std::make_unique<EmergencyBill>("Ali", 2000, 4, 500));
    return bills;
}
