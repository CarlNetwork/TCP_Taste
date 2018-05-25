INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
OBJ_DIR=./obj

SRC=${wildcard ${SRC_DIR}/*.c}
OBJ=${patsubst %.c, $(OBJ_DIR)/%.o, ${notdir ${SRC}}}

#用于查看变量的值
#test:
#   echo $(SRC)
#   echo $(OBJ)

TARGET=main
BIN_TARGET=${BIN_DIR}/${TARGET}

CC=gcc
CFLAGS= -g -Wall -I${INC_DIR}

${BIN_TARGET}:${OBJ}
	${CC} ${OBJ} -o $@ $^

${OBJ_DIR}/%.o:${SRC_DIR}/%.c
	${CC} ${CFLAGS} -c $< -o $@

clean:
	find ${OBJ_DIR} -name *.o -exec rm -rf {} \;
	 

