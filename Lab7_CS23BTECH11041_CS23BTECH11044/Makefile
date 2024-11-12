#making the all target which takes riscv_sim as dependency.
all: riscv_sim 

#making the riscv_sim target which takes all the .cpp files as dependency.
riscv_sim:  main.o assembler_gen_func.o simulator_gen_func.o cache_gen_func.o b_format.o i_format.o j_format.o r_format.o s_format.o u_format.o hexcode_generator.o
	g++ main.o assembler_gen_func.o simulator_gen_func.o cache_gen_func.o b_format.o i_format.o j_format.o r_format.o s_format.o u_format.o hexcode_generator.o -o riscv_sim

main.o: main.cpp
	g++ -c main.cpp

assembler_gen_func.o: assembler_gen_func.cpp
	g++ -c assembler_gen_func.cpp

simulator_gen_func.o: simulator_gen_func.cpp
	g++ -c simulator_gen_func.cpp

cache_gen_func.o: cache_gen_func.cpp
	g++ -c cache_gen_func.cpp

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

hexcode_generator.o: hexcode_generator.cpp
	g++ -c hexcode_generator.cpp

#declared clean as PHONY
.PHONY: clean #so that if there is a file named clean in the client's system still this executes make clean

clean: #the clean target has no dependencies, is called only when explicitly called from the terminal. 'clean's all the executable files made.
	rm -f *.o
	rm -f riscv_sim