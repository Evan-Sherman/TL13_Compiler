echo "Correct input: "
./compiler < ./input_files/correct_input.txt
echo "Bad Syntac: "
./compiler < ./input_files/bad_syntax.txt
echo "Duplicate Declatation: "
./compiler < ./input_files/dup_dec.txt
echo "Mismatch Type: "
./compiler < ./input_files/mismatch_type.txt
echo "Wrong expression type: "
./compiler < ./input_files/wrong_exp_type.txt
echo "Undeclared Var: "
./compiler < ./input_files/undeclared_input.txt

