CC := g++
CLFLAGS := -std=c++11 -O3
CLFLAGS += -MMD
ODIR := .OBJ

_SRC := $(shell find . -name "*.cpp")
ALL_OBJ := $(patsubst %.cpp, $(ODIR)/%.o, $(notdir $(_SRC)))
MAINS := $(ODIR)/coordinator.o $(ODIR)/coach.o $(ODIR)/sorter.o
OBJ := $(filter-out $(MAINS), $(ALL_OBJ))
DEPS := $(patsubst %.cpp, $(ODIR)/%.d, $(notdir $(_SRC)))

$(ODIR):
	@mkdir $(ODIR)

$(ODIR)/%.o: %.cpp
	$(CC) $(CLFLAGS) -c $< -o $@

all: coordinator coach sorter

coordinator: $(ODIR) $(ALL_OBJ)
	$(CC) $(OBJ) $(ODIR)/coordinator.o -o $@

coach: $(ODIR) $(ALL_OBJ)
	$(CC) $(OBJ) $(ODIR)/coach.o -o $@

sorter: $(ODIR) $(ALL_OBJ)
	$(CC) $(OBJ) $(ODIR)/sorter.o -o $@

.PHONY: clean

clean:
	rm -rf coordinator coach sorter $(ODIR)

-include $(DEPS)
