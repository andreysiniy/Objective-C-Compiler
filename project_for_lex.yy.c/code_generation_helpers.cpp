#include "code_generation_helpers.h"

//Инициализация констант
char CodeGenerationHelpers::magicConstant[4] = { 0xCA, 0xFE, 0xBA, 0xBE };
char CodeGenerationHelpers::smallVersion[2] = { 0x00, 0x00 };
char CodeGenerationHelpers::bigVersion[2] = { 0x00, 0x3E };

vector<char> CodeGenerationHelpers::intToByteArray(int num, int arraySize)
{
	vector<char> result(arraySize);
	for (int i = 0; i < arraySize; i++)
	{
		result[arraySize - i - 1] = (num >> ( i * 8));
	}
	return result;
}
// -------------------- ГЕНЕРАЦИИ КОМАНД JVM --------------------

// ---------- iconst, bipush, sipush ----------
vector<char> CodeGenerationCommands::iconstBipushSipush(int num)
{
	vector<char> res;

	// iconst
	if (num == -1) {
		res.push_back(0x2); //iconst_m1
	}
	else if (num == 0) {
		res.push_back(0x3); //iconst_0
	}
	else if (num == 1) {
		res.push_back(0x4); //iconst_1
	}
	else if (num == 2) {
		res.push_back(0x5); //iconst_2
	}
	else if (num == 3) {
		res.push_back(0x6); //iconst_3
	}
	else if (num == 4) {
		res.push_back(0x7); //iconst_4
	}
	else if (num == 5) {
		res.push_back(0x8); //iconst_5
	}

	if (num <= 5 && num >= -1) {
		return res;
	}

	if (num >= -128 && num <= 127) {
		// bipush
		res.push_back(0x10); //bipush
		res.push_back(num);
		return res;
	}

	if (num >= -32768 && num <= 32767) {
		// sipush
		res.push_back(0x11); //sipush
		vector <char> temp = CodeGenerationHelpers::intToByteArray(num, 2);
		CodeGenerationHelpers::appendArrayToByteVector(&res, temp.data(), temp.size());
		return res;
	}
	else {
		throw std::exception("Error in iconstBipushSipush: Invalid number");
	}
}
