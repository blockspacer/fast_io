#include"../../include/fast_io.h"
#include"../../include/fast_io_device.h"

int main()
{
	fast_io::obinary_to_text<fast_io::obuf> text_obuf("a.txt");
	std::size_t val(0);
	println(text_obuf,val);
}