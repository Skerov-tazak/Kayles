CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17
LDFLAGS = 

SERVER_SOURCES = main.cpp ServerController.cpp ServerService.cpp GameStatesArray.cpp GameState.cpp MessageMapper.cpp
CLIENT_SOURCES = client.cpp GameState.cpp MessageMapper.cpp
TEST_SOURCES = test.cpp
DS_TEST_SOURCES = data_structure_test.cpp GameStatesArray.cpp GameState.cpp

SERVER_OBJECTS = $(SERVER_SOURCES:.cpp=.o)
CLIENT_OBJECTS = $(CLIENT_SOURCES:.cpp=.o)
TEST_OBJECTS = test.o
DS_TEST_OBJECTS = data_structure_test.o GameStatesArray.o GameState.o

ALL_OBJECTS = $(sort $(SERVER_OBJECTS) $(CLIENT_OBJECTS) $(TEST_OBJECTS) data_structure_test.o)

.PHONY: all clean tests

all: kayles_server kayles_client tests

tests: test ds_test

kayles_server: $(SERVER_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

kayles_client: $(CLIENT_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test: $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

ds_test: $(DS_TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Header dependencies
main.o: main.cpp ServerController.h
ServerController.o: ServerController.cpp ServerController.h MessageMapper.h ServerService.h kayles_types.h
ServerService.o: ServerService.cpp ServerService.h GameStatesArray.h GameState.h kayles_types.h
GameStatesArray.o: GameStatesArray.cpp GameStatesArray.h GameState.h
GameState.o: GameState.cpp GameState.h kayles_types.h
MessageMapper.o: MessageMapper.cpp MessageMapper.h kayles_types.h GameState.h
client.o: client.cpp kayles_types.h GameState.h MessageMapper.h
test.o: test.cpp kayles_types.h
data_structure_test.o: data_structure_test.cpp GameStatesArray.h GameState.h kayles_types.h

clean:
	rm -f $(ALL_OBJECTS) kayles_server kayles_client test ds_test
