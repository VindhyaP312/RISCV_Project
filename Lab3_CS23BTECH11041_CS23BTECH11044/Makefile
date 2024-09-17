#making the all target which takes riscv_asm as dependency.
all: riscv_asm 

#making the riscv_asm target which takes all the .cpp files as dependency.
riscv_asm:  main.o gen_func.o b_format.o i_format.o j_format.o r_format.o s_format.o u_format.o
	g++ main.o gen_func.o b_format.o i_format.o j_format.o r_format.o s_format.o u_format.o -o riscv_asm

main.o: main.cpp
	g++ -c main.cpp

gen_func.o: gen_func.cpp
	g++ -c gen_func.cpp

b_format.o: b_format.cpp
	g++ -c b_format.cpp

i_format.o: i_format.cpp
	g++ -c i_format.cpp

j_format.o: j_format.cpp
	g++ -c j_format.cpp

r_format.o: r_format.cpp
	g++ -c r_format.cpp

s_format.o: s_format.cpp
	g++ -c s_format.cpp

u_format.o: u_format.cpp
	g++ -c u_format.cpp

#declared clean as PHONY
.PHONY: clean #so that if there is a file named clean in the client's system still this executes make clean

clean: #the clean target has no dependencies, is called only when explicitly called from the terminal. 'clean's all the executable files made.
	rm -f *.o
	rm -f riscv_asm