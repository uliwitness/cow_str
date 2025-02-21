// Copyright 2025 Uli Kusterer.
// Licensed under the zlib license.

#include <iostream>
#include "cow_str.hpp"

using namespace cow;
using namespace std;

int main(int argc, const char * argv[]) {
	str myString("Hello world!");
	
	for (auto ch : myString) {
		cout << ch << endl;
	}
	
	cout << myString << endl;
	cout << "Other string?"_cow << endl;
	
	str collection;
	collection.append("First"_cow);
	collection.append("Second"_cow);
	collection.append("THIRD!!!"_cow);

	cout << collection << endl;
	
	return 0;
}
