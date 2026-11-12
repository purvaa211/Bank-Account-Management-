# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# Target executable
TARGET = smartbank

# Source and object files
SRCS = Account.cpp Bank.cpp BankDatabase.cpp BankStatementGenerator.cpp CurrentAccount.cpp Customer.cpp FixedDepositAccount.cpp FraudDetector.cpp SavingsAccount.cpp Transaction.cpp UIHelper.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)

# Default rule
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe bank_management.exe

# Run rule
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
