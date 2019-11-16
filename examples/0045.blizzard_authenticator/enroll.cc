#include"../../include/fast_io.h"
#include"../../include/fast_io_device.h"
#include"../../include/fast_io_network.h"
#include"../../include/fast_io_crypto.h"
#include"domain.h"
#include<random>

int main(int argc, char** argv)
try
{
	if(argc!=3)
	{
		println(fast_io::err,"Usage: ",*argv," <US/KR/EU/TW/CN> [stored file path]");
		return 1;
	}
	auto const region_array(battlenet::upper_case_region(argv[1]));
	std::string_view const region(region_array.data(),region_array.size());
	auto const domain(battlenet::get_domain(region));
	std::random_device device;
	std::uniform_int_distribution<std::uint8_t> dis(0,255);
	std::array<std::uint8_t,38> one_time_pad_key;
	for(std::size_t i(0);i!=20;++i)
		one_time_pad_key[i]=dis(device);
	std::copy_n(region_array.data(),2,one_time_pad_key.data()+20);
	std::uniform_int_distribution<std::uint16_t> ud(0,62);
	for(std::size_t i(22);i<one_time_pad_key.size();++i)
	{
		std::uint8_t ch(ud(device));
		if(!ch)
			one_time_pad_key[i]=' ';
		else if(ch<11)
			one_time_pad_key[i]=ch+'0';
		else if(ch<37)
			one_time_pad_key[i]=ch-11+'a';
		else
			one_time_pad_key[i]=ch-37+'A';
	}
	fast_io::natural modules;
	{
	fast_io::istring_view isv("955e4bd989f3917d2f15544a7e0504eb9d7bb66b6f8a2fe470e453c779200e5e"
			"3ad2e43a02d06c4adbd8d328f1a426b83658e88bfd949b2af4eaf30054673a14"
			"19a250fa4cc1278d12855b5b25818d162c6e6ee2ab4a350d401d78f6ddb99711"
			"e72626b48bd8b5b0b7f3acf9ea3c9e0005fee59e19136cdb7c83f2ab8b0a2a99");
	scan(isv,fast_io::hex(modules));
	}
	fast_io::natural exponent("257");
	fast_io::natural p;
	p.vec().resize(5);
	std::memcpy(p.vec().data(),std::addressof(one_time_pad_key),38);
	auto result(pow_mod(p,exponent,modules));
	fast_io::client_buf hd(fast_io::dns_once(domain),80,fast_io::sock::type::stream);
	print(hd,"POST ",battlenet::enroll_path," HTTP/1.1\r\n",
		"Content-Type: application/octet-stream\r\n"
		"Content-Length: ",result.vec().size()*8,"\r\n"
		"Timeout: 10000\r\n\r\n");
	writes(hd,result.vec().cbegin(),result.vec().cend());
	fast_io::osystem_file buffer_file("buf.txt");
	print(buffer_file,"One Time Pad: ");
	writes(buffer_file,one_time_pad_key.cbegin(),one_time_pad_key.cend());
	println(buffer_file,"\n\n\n\n\n");
	transmit(buffer_file,hd);
}
catch(std::exception const& e)
{
	println(fast_io::err,e);
	return 1;
}