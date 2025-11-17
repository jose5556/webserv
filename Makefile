NAME     := webserv
T_NAME   := web_tester
CXX      := c++ 
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -I./include

SRC_DIR  := srcs
OBJ_DIR  := obj

		
SRCS_ALL :=	\
		$(SRC_DIR)/cgi/CgiHandler.cpp \
		$(SRC_DIR)/config/Config.cpp $(SRC_DIR)/config/LocationConfig.cpp $(SRC_DIR)/config/ServerConfig.cpp \
		$(SRC_DIR)/core/Connection.cpp $(SRC_DIR)/core/EventLoop.cpp $(SRC_DIR)/core/Server.cpp \
		$(SRC_DIR)/http/HttpParser.cpp $(SRC_DIR)/http/HttpResponse.cpp $(SRC_DIR)/http/HttpParser_throw.cpp \
		$(SRC_DIR)/net/Socket.cpp \



SRCS := $(SRC_DIR)/main.cpp  $(SRCS_ALL)

SRCS_T := ./test/main.cpp  

# Convert source files to object files
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each .cpp into .o, creating necessary directories
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)
	@rm -f $(T_NAME)
	@rm -f leaks.out

re: fclean all

leaks:
	$(CXX) $(CXXFLAGS) -fsanitize=address -g $(SRCS) -o leaks.out
	./leaks.out
tests:
	$(CXX) $(CXXFLAGS) -g $(SRCS_T) $(SRCS_ALL) -o $(T_NAME)
	./$(T_NAME)
	
run:
	make re && ./webserv ./test/youpi.conf
