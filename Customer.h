#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>

/**
 * ============================================================================
 * OOP CONCEPT: COMPOSITION & ENCAPSULATION
 * Customer represents a banking customer entity.
 * Bank and Account compose Customer information.
 * ============================================================================
 */
class Customer
{
private:
    int customerId;
    std::string fullName;
    std::string email;
    std::string phone;

public:
    Customer();
    Customer(int id, const std::string& name, const std::string& mail = "", const std::string& ph = "");

    int getCustomerId() const;
    std::string getFullName() const;
    std::string getEmail() const;
    std::string getPhone() const;

    void setEmail(const std::string& mail);
    void setPhone(const std::string& ph);
};

#endif // CUSTOMER_H
