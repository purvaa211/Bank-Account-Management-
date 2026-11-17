#include "Customer.h"
#include "UIHelper.h"
#include <iomanip>

int Customer::nextCustomerId = 201;

Customer::Customer()
    : customerId(0), fullName(""), email(""), phone("")
{
}

Customer::Customer(const std::string& name, const std::string& mail, const std::string& ph)
    : customerId(nextCustomerId++), fullName(name), email(mail), phone(ph)
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

void Customer::setFullName(const std::string& name)
{
    fullName = name;
}

void Customer::setEmail(const std::string& mail)
{
    email = mail;
}

void Customer::setPhone(const std::string& ph)
{
    phone = ph;
}

void Customer::display() const
{
    std::cout << "\n" << UI::Color::BOLD_CYAN;
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "|                     CUSTOMER PROFILE CARD                   |\n";
    std::cout << "+------------------------------+------------------------------+\n" << UI::Color::RESET;

    auto printField = [](const std::string& label, const std::string& val, const std::string& color = UI::Color::BOLD_WHITE) {
        std::cout << UI::Color::CYAN << "| " << UI::Color::WHITE << std::left << std::setw(28) << label 
                  << UI::Color::CYAN << "| " << color << std::left << std::setw(28) << val 
                  << UI::Color::CYAN << "|\n" << UI::Color::RESET;
    };

    printField("Customer ID", "CUST-" + std::to_string(customerId), UI::Color::BOLD_YELLOW);
    printField("Full Name", fullName);
    printField("Email Address", email.empty() ? "N/A" : email);
    printField("Contact Phone", phone.empty() ? "N/A" : phone);

    std::cout << UI::Color::BOLD_CYAN;
    std::cout << "+------------------------------+------------------------------+\n";
    std::cout << UI::Color::RESET;
}

std::ostream& operator<<(std::ostream& os, const Customer& c)
{
    os << "CUST-" << c.customerId << " | " << c.fullName;
    if (!c.email.empty()) os << " | " << c.email;
    if (!c.phone.empty()) os << " | " << c.phone;
    return os;
}

int Customer::getNextCustomerId()
{
    return nextCustomerId;
}

void Customer::setNextCustomerId(int id)
{
    nextCustomerId = id;
}
