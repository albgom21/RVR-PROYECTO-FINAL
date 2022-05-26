
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class Resources
{
public:
	enum ID : int {
		Pug
	};

	struct IMG {
		ID id;
		string route;
	};

	static vector<IMG> imageRoutes;


};