
CFLAGS+=
LDFLAGS+=-lcurlpp -lcurl -lpthread -ldl -lrt -lPocoJSON -lPocoFoundation

OBJ = main.o

all: $(OBJ)
	$(CXX) -o tprf $(OBJ) $(LDFLAGS)

clean:
	rm tprf || true

