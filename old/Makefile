NAME = libftpp.a
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++11
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
INCLUDE = -I include

all: $(NAME)

$(NAME): $(OBJ)
	ar rcs $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

test_memento:
	$(CXX) $(CXXFLAGS) $(INCLUDE) tests/memento_test.cpp $(NAME) -pthread -o tests/memento_test

test_observer:
	$(CXX) $(CXXFLAGS) $(INCLUDE) tests/observer_test.cpp $(NAME) -pthread -o tests/observer_test

test_singleton:
	$(CXX) $(CXXFLAGS) $(INCLUDE) tests/singleton_test.cpp $(NAME) -pthread -o tests/singleton_test

test_state_machine:
	$(CXX) $(CXXFLAGS) $(INCLUDE) tests/state_machine_test.cpp $(NAME) -pthread -o tests/state_machine_test

test_thread_safe_queue:
	$(CXX) $(CXXFLAGS) $(INCLUDE) tests/thread_safe_queue_test.cpp $(NAME) -pthread -o tests/thread_safe_queue_test

test_worker_pool:
	$(CXX) $(CXXFLAGS) $(INCLUDE) tests/worker_pool_test.cpp $(NAME) -pthread -o tests/worker_pool_test

test_persistent_worker:
	$(CXX) $(CXXFLAGS) $(INCLUDE) tests/persistent_worker_test.cpp $(NAME) -pthread -o tests/persistent_worker_test

test_message:
	$(CXX) $(CXXFLAGS) $(INCLUDE) tests/message_test.cpp $(NAME) -pthread -o tests/message_test

test_network_test:
	$(CXX) $(CXXFLAGS) $(INCLUDE) tests/network_test.cpp $(NAME) -pthread -o tests/network_test
