CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++11 -Iinclude
SRCS = src/data_buffer.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = libftpp.a
AR = ar rcs

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
