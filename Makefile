# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# Target executable
TARGET = bank_management

# Source and object files
SRCS = Account.cpp Bank.cpp CurrentAccount.cpp SavingsAccount.cpp UIHelper.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)

# Default rule
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe accounts.txt

# Run rule
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
