import <print>;
import <iostream>;
import <string_view>;
import <vector>;

import main.dispatch;

int main(int argc, char** argv)
{
	mains::dispatcher disp{argc, argv};
	return disp();
}
