#include<iostream>
using namespace std;
#include<sstream>
#include<string>
std::ostringstream out;

void printXin(){
	float y, x, a;
	for (y = 1.3f; y > -1.1f; y -= 0.1f) {
		for (x = -1.5f; x < 1.5f; x += 0.05f) {
			a = x * x + y * y - 1;
			out<<(a * a * a - x * x * y * y * y <= 0.0f ? "_" : "#");
		}
		out<<"<br>";
	}
}

extern "C" std::string __FUN__(){
out<<R"(
)";out<<R"(
)";out<<R"(
<HTML><HEAD>
)";out<<R"(
</HEAD>
<BODY>
心形图案输出<br>
)";printXin();out<<R"(
<br>
<table border=1 width=80% >
  <tr><th colspan="26">大、小写英文字母表</th></tr>
  
<tr color="blue">
)";
  for (char c='a';c<='z';c++ )
   {
out<<R"(
	<td style="color:blue">)";out<<c;out<<R"(</td>
)";
   }
out<<R"(
</tr>
<tr>
)";
  for (char c='A';c<='Z';c++ )
   {
out<<R"(
	<td style="color:red">)";out<<c;out<<R"(</td>
)";
   }
out<<R"(
</tr> 
  
 </table>
 <br>
计算三角形面积<br>
<FORM action="2.csp" method=post>
  <INPUT type="text" name="a"></p>
   <INPUT type="text" name="b"></p>
    <INPUT type="text" name="c"></p>
  <br><INPUT type="submit" value="send" name=submit>
</FORM>

</tr> 
</table>
</BODY>
</HTML>)";return out.str();}