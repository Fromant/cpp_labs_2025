#include <iostream>
#include <array>
#include <cmath>
#include <fstream>

std::array<double, 3> read_polynom_from_file() {
	std::array<double, 3> to_return{};
	std::ifstream input("input.txt");

	if (!input.is_open()) {
		std::cerr << "Error: Could not open input file" << std::endl;
		exit(1);
	}

	input >> to_return[0] >> to_return[1] >> to_return[2];
	return to_return;
}

void solve_and_write_equation(const std::array<double, 3>& equation) {
	std::ofstream output("output.txt");

	if (!output.is_open()) {
		std::cerr << "Error: Could not open output file" << std::endl;
		exit(1);
	}

	// Count discriminant
	double discr = equation[1]*equation[1]-4*equation[0]*equation[2];

	if(discr < 0) {
		output << "There is no roots in real numbers" << std::endl;
		return;
	}

	if(discr == 0) {
		double root = (-equation[1]+std::sqrt(discr))/(2*equation[0]);
		output << "There is only one root in real numbers: " << root << std::endl;
		return;
	}

	// 2 roots
	double sqrt_discr = std::sqrt(discr);
	double denominator = 2*equation[0];
	double root1 = (-equation[1] + sqrt_discr) / denominator;
	double root2 = (-equation[1] - sqrt_discr) / denominator;
	output << "There are two real roots: " << root1 << "; " << root2 << std::endl;
}

int main() {
	std::cout << "Lab 1 by Krutyanscky RI" << std::endl;
	const auto eq = read_polynom_from_file();
	solve_and_write_equation(eq);
	return 0;
}