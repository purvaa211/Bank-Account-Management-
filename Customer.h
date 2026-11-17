#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include <iostream>

/**
 * ============================================================================
 * OOP CONCEPT: COMPOSITION, ENCAPSULATION & STATIC MEMBERS
 * Customer represents a banking customer entity.
 * Demonstrates:
 *   - Encapsulation: Private personal data with validated accessors.
 *   - Static Member: Auto-incrementing unique customerId generator.
 *   - Operator Overloading: Stream output operator for easy printing.
 * ============================================================================
 */
class Customer
{
private:
    int customerId;
    std::string fullName;
    std::string email;
    std::string phone;

    static int nextCustomerId;

public:
    Customer();
    Customer(const std::string& name, const std::string& mail = "", const std::string& ph = "");
    Customer(int id, const std::string& name, const std::string& mail, const std::string& ph);

    // Getters (Encapsulation)
    int getCustomerId() const;
    std::string getFullName() const;
    std::string getEmail() const;
    std::string getPhone() const;

    // Setters
    void setFullName(const std::string& name);
    void setEmail(const std::string& mail);
    void setPhone(const std::string& ph);

    // Formatted display
    void display() const;

    // Stream operator
    friend std::ostream& operator<<(std::ostream& os, const Customer& c);

    // Static counter management
    static int getNextCustomerId();
    static void setNextCustomerId(int id);
};

#endif // CUSTOMER_H
