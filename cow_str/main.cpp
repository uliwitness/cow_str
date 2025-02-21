// Copyright 2025 Uli Kusterer.
// Licensed under the zlib license.

#include <iostream>
#include "cow_str.hpp"

using namespace cow;
using namespace std;

int main(int argc, const char * argv[]) {
	str myString("Hello world!"_cow);
	
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
	
	str insertableString("future"_cow);
	insertableString.insert(insertableString.begin(), "NO"_cow);
	cout << insertableString << endl;
	
	str insertableString2("helloworld"_cow);
	insertableString2.insert(insertableString2.begin() + 5, ", "_cow);
	cout << insertableString2 << endl;
	
	str insertableString3("Johann Sebastian"_cow);
	insertableString3.insert(insertableString3.end(), " BACH"_cow);
	cout << insertableString3 << endl;
	
	str insertableString4;
	insertableString4.insert(insertableString4.begin(), "HyperCard"_cow);
	cout << insertableString4 << endl;

	str insertableString5;
	insertableString5.insert(insertableString5.end(), "SuperCard"_cow);
	cout << insertableString5 << endl;

	return 0;
}
