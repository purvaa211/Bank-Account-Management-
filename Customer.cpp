#include "Customer.h"

Customer::Customer()
    : customerId(0), fullName(""), email(""), phone("")
{
}

Customer::Customer(int id, const std::string& name, const std::string& mail, const std::string& ph)
    : customerId(id), fullName(name), email(mail), phone(ph)
{
}

int Customer::getCustomerId() const
{
    return customerId;
}

std::string Customer::getFullName() const
{
    return fullName;
}

std::string Customer::getEmail() const
{
    return email;
}

std::string Customer::getPhone() const
{
    return phone;
}

void Customer::setEmail(const std::string& mail)
{
    email = mail;
}

void Customer::setPhone(const std::string& ph)
{
    phone = ph;
}
