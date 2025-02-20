#include <iostream>
#include <array>
#include <cmath>

std::array<double, 3> read_polynom_from_stdin() {
	std::array<double, 3> to_return{};
	std::cout << "Quadratic equation ax^2+bx+c=0" << std::endl;
	std::cout << "Write \"a\" coefficient numeric value: ";
	std::cin >> to_return[0];
	std::cout << "Write \"b\" coefficient numeric value: ";
	std::cin >> to_return[1];
	std::cout << "Write \"c\" coefficient numeric value: ";
	std::cin >> to_return[2];
	return to_return;
}

void solve_and_print_equation(const std::array<double, 3>& equation) {
	//count discriminant
	double discr = equation[1]*equation[1]-4*equation[0]*equation[2];
	if(discr<0) {
		std::cout << "There is no roots in real numbers" << std::endl;
		return;
	}
	if(discr==0) {
		double root = (-equation[1]+std::sqrt(discr))/2.0;
		std::cout << "There is only one root in real numbers: " << root<< std::endl;
		return;
	}
	//2 roots
	double sqrt_discr_halfed = std::sqrt(discr)/2;
	double minus_b_halfed = -equation[1]/2;
	double root1 = minus_b_halfed + sqrt_discr_halfed;
	double root2 = minus_b_halfed - sqrt_discr_halfed;
	std::cout << "There are two real roots: " << root1<<"; " << root2<<std::endl;
}

int main() {
	std::cout << "Lab 1 by Krutyanscky RI" << std::endl;
	const auto eq = read_polynom_from_stdin();
	solve_and_print_equation(eq);
	return 0;
}
