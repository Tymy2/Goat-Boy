#include "instructions.h"
#include "Device.h"
#include <iostream>

const uint8_t A=0x0;
const uint8_t F=0x1;
const uint8_t B=0x2;
const uint8_t C=0x3;
const uint8_t D=0x4;
const uint8_t E=0x5;
const uint8_t H=0x6;
const uint8_t L=0x7;
const uint8_t AF=0x0;
const uint8_t BC=0x1;
const uint8_t DE=0x2;
const uint8_t HL=0x3;

bool calc_z(int a){
	return a == 0x0;
}

bool calc_c8(uint16_t a){
	return a > 0xff;
}

bool calc_c16(uint32_t a){
	return a > 0xffff;
}

bool calc_h8(uint8_t a, uint8_t b){
	return (((a & 0xf) + (b & 0xf)) & 0x10) == 0x10; 
}

bool calc_h16(uint16_t a, uint16_t b){
	return (((a & 0xfff) + (b & 0xfff)) & 0x1000) == 0x1000;
}

void halt(Device * device){
	std::cout << "HALT INSTRUCTION CALLED, NOT IMPLEMENTED YET" << std::endl;
}

void ld_rr_d16(Device * device, uint8_t _rr){
	device->cpu.rr[_rr] = device->cpu.fetch_16();
}

void ld_r_d8(Device * device, uint8_t _r){
	device->cpu.r[_r] = device->cpu.fetch();
}

void ld_r_r(Device * device, uint8_t _r_in, uint8_t _r_out){
	device->cpu.r[_r_in] = device->cpu.r[_r_out];
}

void ld_r_memrr(Device * device, uint8_t _r_in, uint8_t _rr_out){
	device->cpu.r[_r_in] = device->mmu.read(device->cpu.rr[_rr_out]);
}

void ld_memrr_r(Device * device, uint8_t _rr_in, uint8_t _r_out){
	device->mmu.write(device->cpu.rr[_rr_in], device->cpu.r[_r_out]);
}

void ld_mema16_sp(Device * device){
	device->mmu.write(device->cpu.fetch_16(), device->cpu.sp);
}

void ld_memrri_r(Device * device, uint8_t _rr_in, uint8_t _r_out){
	device->mmu.write(device->cpu.rr[_rr_in], device->cpu.r[_r_out]);
	device->cpu.r[_r_out]++;
}

void ld_memrrd_r(Device * device, uint8_t _rr_in, uint8_t _r_out){
	device->mmu.write(device->cpu.rr[_rr_in], device->cpu.r[_r_out]);
	device->cpu.r[_r_out]--;
}

void ld_r_memrri(Device * device, uint8_t _r_in, uint8_t _rr_out){
	device->cpu.r[_r_in] = device->mmu.read(device->cpu.rr[_rr_out]);
	device->cpu.rr[_rr_out]++;
}

void ld_r_memrrd(Device * device, uint8_t _r_in, uint8_t _rr_out){
	device->cpu.r[_r_in] = device->mmu.read(device->cpu.rr[_rr_out]);
	device->cpu.rr[_rr_out]--;
}

void ld_memrr_d8(Device * device, uint8_t _rr_in){
	device->mmu.write(device->cpu.rr[_rr_in], device->cpu.fetch());
}

void ld_sp_d16(Device * device){
	device->cpu.sp = device->cpu.fetch_16();
}

void ldh_mema8_r(Device * device, uint8_t _r_in){
	device->mmu.write(0xff00 + device->cpu.fetch(), device->cpu.r[_r_in]);
}

void ldh_r_mema8(Device * device, uint8_t _r_in){
	device->cpu.r[_r_in] = device->mmu.read(0xff00 + device->cpu.fetch());
}

void ld_memc_r(Device * device, uint8_t _r_out){
	bool c = (device->cpu.r[F] >> 4) & 0x1;
	device->mmu.write(0xff00 + c, device->cpu.r[_r_out]);
}

void ld_r_memc(Device * device, uint8_t _r_in){
	bool c = (device->cpu.r[F] >> 4) & 0x1;
	device->cpu.r[_r_in] = device->mmu.read(0xff00+c);
}

void ld_mema16_r(Device * device, uint8_t _r_out){
	device->mmu.write(device->cpu.fetch_16(), device->cpu.r[_r_out]);
}

void ld_r_mema16(Device * device, uint8_t _r_in){
	device->cpu.r[_r_in] = device->mmu.read(device->cpu.fetch_16());
}

void ld_rr_spr8(Device * device, uint8_t _rr_in){
	int8_t addr_value = device->cpu.fetch();
	int sum = device->cpu.sp + addr_value;
	bool h = calc_h16(device->cpu.sp, addr_value);
	bool c = calc_c16(sum);
	device->cpu.rr[_rr_in] = sum;
	device->cpu.set_flags(0b1111, (h << 1) + c);
}

void ld_sp_rr(Device * device, uint8_t _rr_out){
	device->cpu.sp = device->cpu.rr[_rr_out];
}

void add_rr_rr(Device * device, uint8_t _rr_left, uint8_t _rr_right){
	uint32_t value_l = device->cpu.rr[_rr_left];
	uint32_t value_r = device->cpu.rr[_rr_right];
	bool h = calc_h16(value_l, value_r);
	value_l += value_r;
	bool c = calc_c16(value_l);
	device->cpu.rr[_rr_left] = value_l;
	device->cpu.set_flags(0b0111, (h << 1) + c);
}

void add_rr_sp(Device * device, uint8_t _rr_in){
	uint32_t value_l = device->cpu.rr[_rr_in];
	bool h = calc_h16(value_l, device->cpu.sp);
	value_l += device->cpu.sp;
	bool c = calc_c16(value_l);
	device->cpu.rr[_rr_in] = value_l;
	device->cpu.set_flags(0b0111, (h << 1) + c);
}

void add_r_r(Device * device, uint8_t _r_in, uint8_t _r_out){
	bool h = calc_h8(device->cpu.r[_r_in], device->cpu.r[_r_out]);
	uint16_t sum = device->cpu.r[_r_in];
	sum += device->cpu.r[_r_out];
	device->cpu.r[_r_in] = uint8_t(sum);
	bool z = calc_z(device->cpu.r[_r_in]);
	bool c = calc_c8(sum);
	device->cpu.set_flags(0b1111, (z << 3) + (h << 1) + c );
}

void add_r_memrr(Device * device, uint8_t _r_in, uint8_t _rr_out){
	uint8_t memrr = device->mmu.read(device->cpu.rr[_rr_out]);
	bool h = calc_h8(device->cpu.r[_r_in], memrr);
	uint16_t sum = device->cpu.r[_r_in];
	sum += memrr;
	device->cpu.r[_r_in] = uint8_t(sum);
	bool z = calc_z(device->cpu.r[_r_in]);
	bool c = calc_c8(sum);
	device->cpu.set_flags(0b1111, (z << 3) + (h << 1) + c );
}

void add_r_d8(Device * device, uint8_t _r_in){
	uint8_t addr_val = device->cpu.fetch();
	bool h = calc_h8(device->cpu.r[_r_in], addr_val);
	uint16_t sum = device->cpu.r[_r_in];
	sum += addr_val;
	device->cpu.r[_r_in] = uint8_t(sum);
	bool z = calc_z(device->cpu.r[_r_in]);
	bool c = calc_c8(sum);
	device->cpu.set_flags(0b1111, (z << 3) + (h << 1) + c );
}

void add_sp_r8(Device * device){
	int8_t addr_val = int8_t(device->cpu.fetch());
	bool h = calc_h16(device->cpu.sp, addr_val);
	uint32_t sum = device->cpu.sp;
	sum += addr_val;
	device->cpu.sp = uint16_t(sum);
	bool z = calc_z(device->cpu.sp);
	bool c = calc_c16(sum);
	device->cpu.set_flags(0b1111, (h << 1) + c );
}

void adc_r_r(Device * device, uint8_t _r_in, uint8_t _r_out){
	bool c_s = (device->cpu.r[F] >> 4) & 0x1;
	bool h = calc_h8(device->cpu.r[_r_in], device->cpu.r[_r_out] + c_s);
	uint16_t sum = device->cpu.r[_r_in];
	sum += device->cpu.r[_r_out] + c_s;
	device->cpu.r[_r_in] = uint8_t(sum);
	bool z = calc_z(device->cpu.r[_r_in]);
	bool c = calc_c8(sum);
	device->cpu.set_flags(0b1111, (z << 3) + (h << 1) + c );	
}

void adc_r_memrr(Device * device, uint8_t _r_in, uint8_t _rr_out){
	bool c_s = (device->cpu.r[F] >> 4) & 0x1;
	uint16_t rr = device->cpu.rr[_rr_out];
	bool h = calc_h8(device->cpu.r[_r_in], device->mmu.read(rr) + c_s);
	uint16_t sum = device->cpu.r[_r_in];
	sum += device->mmu.read(_rr_out) + c_s;
	device->cpu.r[_r_in] = uint8_t(sum);
	bool z = calc_z(device->cpu.r[_r_in]);
	bool c = calc_c8(sum);
	device->cpu.set_flags(0b1111, (z << 3) + (h << 1) + c );
}

void adc_r_d8(Device * device, uint8_t _r_in){
	bool c_s = (device->cpu.r[F] >> 4) & 0x1;
	uint8_t addr_val = device->cpu.fetch();
	bool h = calc_h8(device->cpu.r[_r_in], addr_val + c_s);
	uint16_t sum = device->cpu.r[_r_in];
	sum += addr_val + c_s;
	device->cpu.r[_r_in] = uint8_t(sum);
	bool z = calc_z(device->cpu.r[_r_in]);
	bool c = calc_c8(sum);
	device->cpu.set_flags(0b1111, (z << 3) + (h << 1) + c );	
}

void sub_r(Device * device, uint8_t _r_in){
	uint16_t res = device->cpu.r[A];
	res -= device->cpu.r[_r_in];
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[A], device->cpu.r[_r_in]);
	bool c = calc_c8(res);
	device->cpu.r[A] = res;
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void sub_memrr(Device * device, uint8_t _rr_out){
	uint16_t rr = device->cpu.rr[_rr_out];
	uint16_t res = device->cpu.r[A];
	res -= device->mmu.read(rr);
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[A], device->mmu.read(rr));
	bool c = calc_c8(res);
	device->cpu.r[A] = res;
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void sub_r_d8(Device * device, uint8_t _r_in){
	uint8_t addr_val = device->cpu.fetch();
	uint16_t res = device->cpu.r[_r_in];
	res -= addr_val;
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[_r_in], addr_val);
	bool c = calc_c8(res);
	device->cpu.r[_r_in] = res;
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void sbc_r_r(Device * device, uint8_t _r_in, uint8_t _r_out){
	uint16_t res = device->cpu.r[_r_in];
	bool c_s = (device->cpu.r[F] >> 4) & 0x1;
	res -= device->cpu.r[_r_out] + c_s;
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[_r_in], device->cpu.r[_r_out]+c_s);
	bool c = calc_c8(res);
	device->cpu.r[_r_in] = res;
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void sbc_r_d8(Device * device, uint8_t _r_in){
	uint8_t addr_val = device->cpu.fetch();
	uint16_t res = device->cpu.r[_r_in];
	bool c_s = (device->cpu.r[F] >> 4) & 0x1;
	res -= addr_val + c_s;
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[_r_in], addr_val+c_s);
	bool c = calc_c8(res);
	device->cpu.r[_r_in] = res;
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void sbc_r_memrr(Device * device, uint8_t _r_in, uint8_t _rr_out){
	uint16_t rr = device->cpu.rr[_rr_out];
	uint16_t res = device->cpu.r[_r_in];
	bool c_s = (device->cpu.r[F] >> 4) & 0x1;
	res -= device->mmu.read(rr) + c_s;
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[_r_in], device->mmu.read(rr)+c_s);
	bool c = calc_c8(res);
	device->cpu.r[_r_in] = res;
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void inc_rr(Device * device, uint8_t _rr_in){
	device->cpu.rr[_rr_in]++;
}

void inc_memrr(Device * device, uint8_t _rr_out){
	uint16_t addr = device->cpu.rr[_rr_out];
	uint8_t value = device->mmu.read(addr);
	bool h = calc_h8(value, value+1);
	value++;
	bool z = calc_z(value);
	device->mmu.write(addr, value);
	device->cpu.set_flags(0b1110, (z << 3) + (h << 1) );
}

void inc_sp(Device * device){
	device->cpu.sp++;
}

void dec_rr(Device * device, uint8_t _rr_in){
	device->cpu.rr[_rr_in]--;
}

void dec_memrr(Device * device, uint8_t _rr_in){
	uint16_t addr = device->cpu.rr[_rr_in];
	uint8_t addr_value = device->mmu.read(addr);
	bool h = calc_h8(addr_value, addr_value-1);
	addr_value--;
	device->mmu.write(addr, addr_value);
	bool z = calc_z(addr_value);
	device->cpu.set_flags(0b1110, (z << 3) + (0x1 << 2) + (h << 1) );
}

void inc_r(Device * device, uint8_t _r_in){
	device->cpu.r[_r_in]++;
	bool z = calc_z(device->cpu.r[_r_in]);
	bool h = calc_h8(device->cpu.r[_r_in], device->cpu.r[_r_in]-0x1);
	device->cpu.set_flags(0b1110, (z << 3)+(h << 1));
}

void dec_sp(Device * device){
	device->cpu.sp--;
}

void dec_r(Device * device, uint8_t _r_in){
	bool h = calc_h8(device->cpu.r[_r_in]-0x1, device->cpu.r[_r_in]);
	device->cpu.r[_r_in]--;
	bool z = calc_z(device->cpu.r[_r_in]);
	device->cpu.set_flags(0b1110, (z << 3)+(0x1 << 2)+(h << 1));
}

void rlca(Device * device){
	uint8_t bit = device->cpu.r[A] >> 7;
	device->cpu.r[A] = (device->cpu.r[A] << 1) | bit;
	device->cpu.set_flags(0b1111, bit);
}

void rla(Device * device){
	bool bit = device->cpu.r[A] >> 7;
	device->cpu.r[A] = 	((device->cpu.r[A] << 1) & 0xff) + ((device->cpu.r[F] >> 4) & 0x1);
	device->cpu.set_flags(0b1111, bit);
}

void rrca(Device * device){
	bool bit = device->cpu.r[A] & 0x1;
	device->cpu.r[A] >>= 1;
	device->cpu.r[A] += bit << 7;
	device->cpu.set_flags(0b1111, bit);
}

void rra(Device * device){
	bool bit = device->cpu.r[A] & 0x1;
	device->cpu.r[A] >>= 1;
	device->cpu.r[A] += ((device->cpu.r[F] >> 4) & 0x1 ) << 7;
	device->cpu.set_flags(0b1111, bit);	
}

void jr_r8(Device * device){
	device->cpu.pc += int8_t(device->cpu.fetch());
}

void jr_nz_r8(Device * device){
	int8_t value = int8_t(device->cpu.fetch());
	if((device->cpu.r[F] >> 7) == 0x0){
		device->cpu.pc += value;
	}
}

void jr_nc_r8(Device * device){
	int8_t value = int8_t(device->cpu.fetch());
	if(((device->cpu.r[F] >> 4) & 0x1) == 0x0){
		device->cpu.pc += value;
	}
}

void jr_c_r8(Device * device){
	int8_t value = int8_t(device->cpu.fetch());
	if(((device->cpu.r[F] >> 4) & 0x1) == 0x1){
		device->cpu.pc += value;
	}
}

void jr_z_r8(Device * device){
	int8_t value = int8_t(device->cpu.fetch());
	if((device->cpu.r[F] >> 7) == 0x1){
		device->cpu.pc += value;
	}
}

void scf(Device * device){
	device->cpu.set_flags(0b0111, 0b0001);
}

void cpl(Device * device){
	device->cpu.r[A] = device->cpu.r[A] ^ 0xff;
	device->cpu.set_flags(0b0110, 0b0110);
}

void ccf(Device * device){
	bool c = ((device->cpu.r[F] >> 4) & 0x1) ^ 0x1;
	device->cpu.set_flags(0b0111, c);
}

void and_r(Device * device, uint8_t _r_out){
	device->cpu.r[A] = device->cpu.r[A] & device->cpu.r[_r_out];
	bool z = calc_z(device->cpu.r[A]);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 1));
}

void and_memrr(Device * device, uint8_t _rr_out){
	uint16_t rr = device->cpu.rr[_rr_out];
	device->cpu.r[A] = device->cpu.r[A] & device->mmu.read(rr);
	bool z = calc_z(device->cpu.r[A]);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 1));
}

void and_r_d8(Device * device, uint8_t _r_in){
	device->cpu.r[_r_in] = device->cpu.r[_r_in] & device->cpu.fetch();
	bool z = calc_z(device->cpu.r[_r_in]);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 1));
}

void xor_r(Device * device, uint8_t _r_in){
	device->cpu.r[A] = device->cpu.r[A] ^ device->cpu.r[_r_in];
	bool z = calc_z(device->cpu.r[A]);
	device->cpu.set_flags(0b1111, z << 3);
}

void xor_r_d8(Device * device, uint8_t _r_in){
	device->cpu.r[_r_in] = device->cpu.r[_r_in] ^ device->cpu.fetch();
	bool z = calc_z(device->cpu.r[_r_in]);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 1));
}

void xor_memrr(Device * device, uint8_t _rr_out){
	uint16_t rr = device->cpu.rr[_rr_out];
	device->cpu.r[A] = device->cpu.r[A] ^ device->mmu.read(rr);
	bool z = calc_z(device->cpu.r[A]);
	device->cpu.set_flags(0b1111, z << 3);
}

void or_r(Device * device, uint8_t _r_out){
	device->cpu.r[A] = device->cpu.r[A] | device->cpu.r[_r_out];
	bool z = calc_z(device->cpu.r[A]);
	device->cpu.set_flags(0b1111, z << 3);
}

void or_memrr(Device * device, uint8_t _rr_out){
	uint16_t rr = device->cpu.rr[_rr_out];
	device->cpu.r[A] = device->cpu.r[A] | device->mmu.read(rr);
	bool z = calc_z(device->cpu.r[A]);
	device->cpu.set_flags(0b1111, z << 3);
}

void or_r_d8(Device * device, uint8_t _r_in){
	device->cpu.r[_r_in] = device->cpu.r[_r_in] | device->cpu.fetch();
	bool z = calc_z(device->cpu.r[_r_in]);
	device->cpu.set_flags(0b1111, z << 3);
}

void cp_r(Device * device, uint8_t _r_out){
	uint16_t res = device->cpu.r[A];
	res -= device->cpu.r[_r_out];
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[A], device->cpu.r[_r_out]);
	bool c = calc_c8(res);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void cp_memrr(Device * device, uint8_t _rr_out){
	uint16_t rr = device->cpu.rr[_rr_out];
	uint16_t res = device->cpu.r[A];
	res -= device->mmu.read(rr);
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[A], device->mmu.read(rr));
	bool c = calc_c8(res);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void cp_d8(Device * device){
	uint8_t addr_val = device->cpu.fetch();
	uint16_t res = device->cpu.r[A];
	res -= addr_val;
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[A], addr_val);
	bool c = calc_c8(res);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void ret_nz(Device * device){
	if((device->cpu.r[F] >> 7) == 0x0){
		device->cpu.pc = device->mmu.read_16(device->cpu.sp);
		device->cpu.sp += 2;
	}
}

void ret_z(Device * device){
	if((device->cpu.r[F] >> 7) == 0x1){
		device->cpu.pc = device->mmu.read_16(device->cpu.sp);
		device->cpu.sp += 2;
	}
}

void ret_nc(Device * device){
	if(((device->cpu.r[F] >> 4) & 0x1) == 0x0){
		device->cpu.pc = device->mmu.read_16(device->cpu.sp);
		device->cpu.sp += 2;
	}
}

void ret_c(Device * device){
	if(((device->cpu.r[F] >> 4) & 0x1) == 0x1){
		device->cpu.pc = device->mmu.read_16(device->cpu.sp);
		device->cpu.sp += 2;
	}
}

void ret(Device * device){
	device->cpu.pc = device->mmu.read_16(device->cpu.sp);
	device->cpu.sp += 2;
}

void reti(Device * device){
	// TODO
}

void pop_rr(Device * device, uint8_t _rr_in){
	device->cpu.rr[_rr_in] = device->mmu.read_16(device->cpu.sp);
	device->cpu.sp += 2;
}


void jp_nz_a16(Device * device){
	uint16_t value = device->cpu.fetch_16();
	if((device->cpu.r[F] >> 7) == 0x0){
		device->cpu.pc = value;
	}
}

void jp_z_a16(Device * device){
	uint16_t value = device->cpu.fetch_16();
	if((device->cpu.r[F] >> 7) == 0x1){
		device->cpu.pc = value;
	}
}

void jp_nc_a16(Device * device){
	uint16_t value = device->cpu.fetch_16();
	if(((device->cpu.r[F] >> 4) & 0x1) == 0x0){
		device->cpu.pc = value;
	}
}

void jp_c_a16(Device * device){
	uint16_t value = device->cpu.fetch_16();
	if(((device->cpu.r[F] >> 4) & 0x1) == 0x1){
		device->cpu.pc = value;
	}
}

void jp_a16(Device * device){
	device->cpu.pc = device->cpu.fetch_16();
}

void jp_memrr(Device * device, uint8_t _rr_out){
	device->cpu.pc = device->cpu.rr[_rr_out];
}

void call_nz_a16(Device * device){
	uint16_t n_pc = device->cpu.fetch_16();
	if((device->cpu.r[F] >> 7) == 0x0){
		device->cpu.sp -= 2;
		device->mmu.write_16(device->cpu.sp, device->cpu.pc);
		device->cpu.pc = n_pc;
	}
}

void call_z_a16(Device * device){
	uint16_t n_pc = device->cpu.fetch_16();
	if((device->cpu.r[F] >> 7) == 0x1){
		device->cpu.sp -= 2;
		device->mmu.write_16(device->cpu.sp, device->cpu.pc);
		device->cpu.pc = n_pc;
	}
}

void call_nc_a16(Device * device){
	uint16_t n_pc = device->cpu.fetch_16();
	if(((device->cpu.r[F] >> 4) & 0x1) == 0x0){
		device->cpu.sp -= 2;
		device->mmu.write_16(device->cpu.sp, device->cpu.pc);
		device->cpu.pc = n_pc;
	}
}

void call_c_a16(Device * device){
	uint16_t n_pc = device->cpu.fetch_16();
	if(((device->cpu.r[F] >> 4) & 0x1) == 0x1){
		device->cpu.sp -= 2;
		device->mmu.write_16(device->cpu.sp, device->cpu.pc);
		device->cpu.pc = n_pc;
	}
}

void call_a16(Device * device){
	int16_t n_pc = device->cpu.fetch_16();
	device->cpu.sp -= 2;
	device->mmu.write_16(device->cpu.sp, device->cpu.pc);
	device->cpu.pc = n_pc;
}

void push_rr(Device * device, uint8_t _rr_out){
	device->cpu.sp -= 2;
	device->mmu.write_16(device->cpu.sp, device->cpu.rr[_rr_out]);
}

void rst(Device * device, uint8_t addr){
	device->cpu.sp -= 2;
	device->mmu.write_16(device->cpu.sp, device->cpu.pc);
	device->cpu.pc = addr;
}

void prefix_cb(Device * device){
	// TODO  this is temporal for bootrom testing only purposes
	uint8_t cb_op = device->cpu.fetch();
	if(cb_op == 0x7c){
		device->cpu.set_flags(0b1110, ( (((device->cpu.r[H] >> 7) & 0x1) ^ 0x1) << 3) + (0x1 << 1) );
	}else if (cb_op == 0x4f){
		device->cpu.set_flags(0b1110, ( (((device->cpu.r[A] >> 1) & 0x1) ^ 0x1) << 3) + (0x1 << 1) );
	}else if (cb_op == 0x11){
		bool c = device->cpu.r[C] >> 7;
		device->cpu.r[C] = ((device->cpu.r[C] << 1) & 0xff) + ((device->cpu.r[F] >> 4) & 0x1);
		bool z = calc_z(device->cpu.r[C]);
		device->cpu.set_flags(0b1111, (z << 3) + c);
	}
}

void illegal_op(Device * device){
	std::cout << "[ERROR] this instruction is ILLEGAL -> " << int(device->mmu.read(device->cpu.pc-1)) << " PC: " << int(device->cpu.pc) << std::endl;
	device->keep_running = false;
}

void di(Device * device){
	//TODO
}

void ei(Device * device){
	//TODO
}

void daa(Device * device){
	//TODO
}

void stop(Device * device){
	device->cpu.fetch();
	device->keep_running = false;
}

void _0x00(Device * device){ /*NOP*/ }
void _0x01(Device * device){ ld_rr_d16(device, BC); }
void _0x02(Device * device){ ld_memrr_r(device, BC, A); }
void _0x03(Device * device){ inc_rr(device, BC); }
void _0x04(Device * device){ inc_r(device, B); }
void _0x05(Device * device){ dec_r(device, B); }
void _0x06(Device * device){ ld_r_d8(device, B); }
void _0x07(Device * device){ rlca(device); }
void _0x08(Device * device){ ld_mema16_sp(device); }
void _0x09(Device * device){ add_rr_rr(device, HL, BC); }
void _0x0a(Device * device){ ld_r_memrr(device, A, BC); }
void _0x0b(Device * device){ dec_rr(device, BC); }
void _0x0c(Device * device){ inc_r(device, C); }
void _0x0d(Device * device){ dec_r(device, C); }
void _0x0e(Device * device){ ld_r_d8(device, C); }
void _0x0f(Device * device){ rrca(device); }
void _0x10(Device * device){ stop(device); }
void _0x11(Device * device){ ld_rr_d16(device, DE); }
void _0x12(Device * device){ ld_memrr_r(device, DE, A); }
void _0x13(Device * device){ inc_rr(device, DE); }
void _0x14(Device * device){ inc_r(device, D); }
void _0x15(Device * device){ dec_r(device, D); }
void _0x16(Device * device){ ld_r_d8(device, D); }
void _0x17(Device * device){ rla(device); }
void _0x18(Device * device){ jr_r8(device); }
void _0x19(Device * device){ add_rr_rr(device, HL, DE); }
void _0x1a(Device * device){ ld_r_memrr(device, A, DE); }
void _0x1b(Device * device){ dec_rr(device, DE); }
void _0x1c(Device * device){ inc_r(device, E); }
void _0x1d(Device * device){ dec_r(device, E); }
void _0x1e(Device * device){ ld_r_d8(device, E); }
void _0x1f(Device * device){ rra(device); }
void _0x20(Device * device){ jr_nz_r8(device); }
void _0x21(Device * device){ ld_rr_d16(device, HL); }
void _0x22(Device * device){ ld_memrri_r(device, HL, A); }
void _0x23(Device * device){ inc_rr(device, HL); }
void _0x24(Device * device){ inc_r(device, H); }
void _0x25(Device * device){ dec_r(device, H); }
void _0x26(Device * device){ ld_r_d8(device, H); }
void _0x27(Device * device){ daa(device); }
void _0x28(Device * device){ jr_z_r8(device); }
void _0x29(Device * device){ add_rr_rr(device, HL, HL); }
void _0x2a(Device * device){ ld_r_memrri(device, A, HL); }
void _0x2b(Device * device){ dec_rr(device, HL); }
void _0x2c(Device * device){ inc_r(device, L); }
void _0x2d(Device * device){ dec_r(device, L); }
void _0x2e(Device * device){ ld_r_d8(device, L); }
void _0x2f(Device * device){ cpl(device); }
void _0x30(Device * device){ jr_nc_r8(device); }
void _0x31(Device * device){ ld_sp_d16(device); }
void _0x32(Device * device){ ld_memrrd_r(device, HL, A); }
void _0x33(Device * device){ inc_sp(device); }
void _0x34(Device * device){ inc_memrr(device, HL); }
void _0x35(Device * device){ dec_memrr(device, HL); }
void _0x36(Device * device){ ld_memrr_d8(device, HL); }
void _0x37(Device * device){ scf(device); }
void _0x38(Device * device){ jr_c_r8(device); }
void _0x39(Device * device){ add_rr_sp(device, HL); }
void _0x3a(Device * device){ ld_r_memrrd(device, A, HL); }
void _0x3b(Device * device){ dec_sp(device); }
void _0x3c(Device * device){ inc_r(device, A); }
void _0x3d(Device * device){ dec_r(device, A); }
void _0x3e(Device * device){ ld_r_d8(device, A); }
void _0x3f(Device * device){ ccf(device); }
void _0x40(Device * device){ ld_r_r(device, B, B); }
void _0x41(Device * device){ ld_r_r(device, B, C); }
void _0x42(Device * device){ ld_r_r(device, B, D); }
void _0x43(Device * device){ ld_r_r(device, B, E); }
void _0x44(Device * device){ ld_r_r(device, B, H); }
void _0x45(Device * device){ ld_r_r(device, B, L); }
void _0x46(Device * device){ ld_r_memrr(device, B, HL); }
void _0x47(Device * device){ ld_r_r(device, B, A); }
void _0x48(Device * device){ ld_r_r(device, C, B); }
void _0x49(Device * device){ ld_r_r(device, C, C); }
void _0x4a(Device * device){ ld_r_r(device, C, D); }
void _0x4b(Device * device){ ld_r_r(device, C, E); }
void _0x4c(Device * device){ ld_r_r(device, C, H); }
void _0x4d(Device * device){ ld_r_r(device, C, L); }
void _0x4e(Device * device){ ld_r_memrr(device, C, HL); }
void _0x4f(Device * device){ ld_r_r(device, C, A); }
void _0x50(Device * device){ ld_r_r(device, D, B); }
void _0x51(Device * device){ ld_r_r(device, D, C); }
void _0x52(Device * device){ ld_r_r(device, D, D); }
void _0x53(Device * device){ ld_r_r(device, D, E); }
void _0x54(Device * device){ ld_r_r(device, D, H); }
void _0x55(Device * device){ ld_r_r(device, D, L); }
void _0x56(Device * device){ ld_r_memrr(device, D, HL); }
void _0x57(Device * device){ ld_r_r(device, D, A); }
void _0x58(Device * device){ ld_r_r(device, E, B); }
void _0x59(Device * device){ ld_r_r(device, E, C); }
void _0x5a(Device * device){ ld_r_r(device, E, D); }
void _0x5b(Device * device){ ld_r_r(device, E, E); }
void _0x5c(Device * device){ ld_r_r(device, E, H); }
void _0x5d(Device * device){ ld_r_r(device, E, L); }
void _0x5e(Device * device){ ld_r_memrr(device, E, HL); }
void _0x5f(Device * device){ ld_r_r(device, E, A); }
void _0x60(Device * device){ ld_r_r(device, H, B); }
void _0x61(Device * device){ ld_r_r(device, H, C); }
void _0x62(Device * device){ ld_r_r(device, H, D); }
void _0x63(Device * device){ ld_r_r(device, H, E); }
void _0x64(Device * device){ ld_r_r(device, H, H); }
void _0x65(Device * device){ ld_r_r(device, H, L); }
void _0x66(Device * device){ ld_r_memrr(device, H, HL); }
void _0x67(Device * device){ ld_r_r(device, H, A); }
void _0x68(Device * device){ ld_r_r(device, L, B); }
void _0x69(Device * device){ ld_r_r(device, L, C); }
void _0x6a(Device * device){ ld_r_r(device, L, D); }
void _0x6b(Device * device){ ld_r_r(device, L, E); }
void _0x6c(Device * device){ ld_r_r(device, L, H); }
void _0x6d(Device * device){ ld_r_r(device, L, L); }
void _0x6e(Device * device){ ld_r_memrr(device, L, HL); }
void _0x6f(Device * device){ ld_r_r(device, L, A); }
void _0x70(Device * device){ ld_memrr_r(device, HL, B); }
void _0x71(Device * device){ ld_memrr_r(device, HL, C); }
void _0x72(Device * device){ ld_memrr_r(device, HL, D); }
void _0x73(Device * device){ ld_memrr_r(device, HL, E); }
void _0x74(Device * device){ ld_memrr_r(device, HL, H); }
void _0x75(Device * device){ ld_memrr_r(device, HL, L); }
void _0x76(Device * device){ halt(device); }
void _0x77(Device * device){ ld_memrr_r(device, HL, A); }
void _0x78(Device * device){ ld_r_r(device, A, B); }
void _0x79(Device * device){ ld_r_r(device, A, C); }
void _0x7a(Device * device){ ld_r_r(device, A, D); }
void _0x7b(Device * device){ ld_r_r(device, A, E); }
void _0x7c(Device * device){ ld_r_r(device, A, H); }
void _0x7d(Device * device){ ld_r_r(device, A, L); }
void _0x7e(Device * device){ ld_r_memrr(device, A, HL); }
void _0x7f(Device * device){ ld_r_r(device, A, A); }
void _0x80(Device * device){ add_r_r(device, A, B); }
void _0x81(Device * device){ add_r_r(device, A, C); }
void _0x82(Device * device){ add_r_r(device, A, D); }
void _0x83(Device * device){ add_r_r(device, A, E); }
void _0x84(Device * device){ add_r_r(device, A, H); }
void _0x85(Device * device){ add_r_r(device, A, L); }
void _0x86(Device * device){ add_r_memrr(device, A, HL); }
void _0x87(Device * device){ add_r_r(device, A, A); }
void _0x88(Device * device){ adc_r_r(device, A, B); }
void _0x89(Device * device){ adc_r_r(device, A, C); }
void _0x8a(Device * device){ adc_r_r(device, A, D); }
void _0x8b(Device * device){ adc_r_r(device, A, E); }
void _0x8c(Device * device){ adc_r_r(device, A, H); }
void _0x8d(Device * device){ adc_r_r(device, A, L); }
void _0x8e(Device * device){ adc_r_memrr(device, A, HL); }
void _0x8f(Device * device){ adc_r_r(device, A, A); }
void _0x90(Device * device){ sub_r(device, B); }
void _0x91(Device * device){ sub_r(device, C); }
void _0x92(Device * device){ sub_r(device, D); }
void _0x93(Device * device){ sub_r(device, E); }
void _0x94(Device * device){ sub_r(device, H); }
void _0x95(Device * device){ sub_r(device, L); }
void _0x96(Device * device){ sub_memrr(device, HL); }
void _0x97(Device * device){ sub_r(device, A); }
void _0x98(Device * device){ sbc_r_r(device, A, B); }
void _0x99(Device * device){ sbc_r_r(device, A, C); }
void _0x9a(Device * device){ sbc_r_r(device, A, D); }
void _0x9b(Device * device){ sbc_r_r(device, A, E); }
void _0x9c(Device * device){ sbc_r_r(device, A, H); }
void _0x9d(Device * device){ sbc_r_r(device, A, L); }
void _0x9e(Device * device){ sbc_r_memrr(device, A, HL); }
void _0x9f(Device * device){ sbc_r_r(device, A, A); }
void _0xa0(Device * device){ and_r(device, B); }
void _0xa1(Device * device){ and_r(device, C); }
void _0xa2(Device * device){ and_r(device, D); }
void _0xa3(Device * device){ and_r(device, E); }
void _0xa4(Device * device){ and_r(device, H); }
void _0xa5(Device * device){ and_r(device, L); }
void _0xa6(Device * device){ and_memrr(device, HL); }
void _0xa7(Device * device){ and_r(device, A); }
void _0xa8(Device * device){ xor_r(device, B); }
void _0xa9(Device * device){ xor_r(device, C); }
void _0xaa(Device * device){ xor_r(device, D); }
void _0xab(Device * device){ xor_r(device, E); }
void _0xac(Device * device){ xor_r(device, H); }
void _0xad(Device * device){ xor_r(device, L); }
void _0xae(Device * device){ xor_memrr(device, HL); }
void _0xaf(Device * device){ xor_r(device, A); }
void _0xb0(Device * device){ or_r(device, B); }
void _0xb1(Device * device){ or_r(device, C); }
void _0xb2(Device * device){ or_r(device, D); }
void _0xb3(Device * device){ or_r(device, E); }
void _0xb4(Device * device){ or_r(device, H); }
void _0xb5(Device * device){ or_r(device, L); }
void _0xb6(Device * device){ or_memrr(device, HL); }
void _0xb7(Device * device){ or_r(device, A); }
void _0xb8(Device * device){ cp_r(device, B); }
void _0xb9(Device * device){ cp_r(device, C); }
void _0xba(Device * device){ cp_r(device, D); }
void _0xbb(Device * device){ cp_r(device, E); }
void _0xbc(Device * device){ cp_r(device, H); }
void _0xbd(Device * device){ cp_r(device, L); }
void _0xbe(Device * device){ cp_memrr(device, HL); }
void _0xbf(Device * device){ cp_r(device, A); }
void _0xc0(Device * device){ ret_nz(device); }
void _0xc1(Device * device){ pop_rr(device, BC); }
void _0xc2(Device * device){ jp_nz_a16(device); }
void _0xc3(Device * device){ jp_a16(device); }
void _0xc4(Device * device){ call_nz_a16(device); }
void _0xc5(Device * device){ push_rr(device, BC); }
void _0xc6(Device * device){ add_r_d8(device, A); }
void _0xc7(Device * device){ rst(device, 0x00); }
void _0xc8(Device * device){ ret_z(device); }
void _0xc9(Device * device){ ret(device); }
void _0xca(Device * device){ jp_z_a16(device); }
void _0xcb(Device * device){ prefix_cb(device); }
void _0xcc(Device * device){ call_z_a16(device); }
void _0xcd(Device * device){ call_a16(device); }
void _0xce(Device * device){ adc_r_d8(device, A); }
void _0xcf(Device * device){ rst(device, 0x08); }
void _0xd0(Device * device){ ret_nc(device); }
void _0xd1(Device * device){ pop_rr(device, DE); }
void _0xd2(Device * device){ jp_nc_a16(device); }
void _0xd3(Device * device){ illegal_op(device); }
void _0xd4(Device * device){ call_nc_a16(device); }
void _0xd5(Device * device){ push_rr(device, DE); }
void _0xd6(Device * device){ sub_r_d8(device, A); }
void _0xd7(Device * device){ rst(device, 0x10); }
void _0xd8(Device * device){ ret_c(device); }
void _0xd9(Device * device){ reti(device); }
void _0xda(Device * device){ jp_c_a16(device); }
void _0xdb(Device * device){ illegal_op(device); }
void _0xdc(Device * device){ call_c_a16(device); }
void _0xdd(Device * device){ illegal_op(device); }
void _0xde(Device * device){ sbc_r_d8(device, A); }
void _0xdf(Device * device){ rst(device, 0x18); }
void _0xe0(Device * device){ ldh_mema8_r(device, A); }
void _0xe1(Device * device){ pop_rr(device, HL); }
void _0xe2(Device * device){ ld_memc_r(device, A); }
void _0xe3(Device * device){ illegal_op(device); }
void _0xe4(Device * device){ illegal_op(device); }
void _0xe5(Device * device){ push_rr(device, HL); }
void _0xe6(Device * device){ and_r_d8(device, A); }
void _0xe7(Device * device){ rst(device, 0x20); }
void _0xe8(Device * device){ add_sp_r8(device); }
void _0xe9(Device * device){ jp_memrr(device, HL); }
void _0xea(Device * device){ ld_mema16_r(device, A); }
void _0xeb(Device * device){ illegal_op(device); }
void _0xec(Device * device){ illegal_op(device); }
void _0xed(Device * device){ illegal_op(device); }
void _0xee(Device * device){ xor_r_d8(device, A); }
void _0xef(Device * device){ rst(device, 0x28); }
void _0xf0(Device * device){ ldh_r_mema8(device, A); }
void _0xf1(Device * device){ pop_rr(device, AF);  }
void _0xf2(Device * device){ ld_r_memc(device, A); }
void _0xf3(Device * device){ di(device); }
void _0xf4(Device * device){ illegal_op(device); }
void _0xf5(Device * device){ push_rr(device, AF); }
void _0xf6(Device * device){ or_r_d8(device, A); }
void _0xf7(Device * device){ rst(device, 0x30); }
void _0xf8(Device * device){ ld_rr_spr8(device, HL); }
void _0xf9(Device * device){ ld_sp_rr(device, HL); }
void _0xfa(Device * device){ ld_r_mema16(device, A); }
void _0xfb(Device * device){ ei(device); }
void _0xfc(Device * device){ illegal_op(device); }
void _0xfd(Device * device){ illegal_op(device); }
void _0xfe(Device * device){ cp_d8(device); }
void _0xff(Device * device){ rst(device, 0x38); }


void setup_instructions(Device * device){
	void (* instructions[0x100])(Device *) = {
	&_0x00, &_0x01, &_0x02, &_0x03, &_0x04, &_0x05, &_0x06, &_0x07, &_0x08, &_0x09, &_0x0a, &_0x0b, &_0x0c, &_0x0d, &_0x0e, &_0x0f,
	&_0x10, &_0x11, &_0x12, &_0x13, &_0x14, &_0x15, &_0x16, &_0x17, &_0x18, &_0x19, &_0x1a, &_0x1b, &_0x1c, &_0x1d, &_0x1e, &_0x1f,
	&_0x20, &_0x21, &_0x22, &_0x23, &_0x24, &_0x25, &_0x26, &_0x27, &_0x28, &_0x29, &_0x2a, &_0x2b, &_0x2c, &_0x2d, &_0x2e, &_0x2f,
 	&_0x30, &_0x31, &_0x32, &_0x33, &_0x34, &_0x35, &_0x36, &_0x37, &_0x38, &_0x39, &_0x3a, &_0x3b, &_0x3c, &_0x3d, &_0x3e, &_0x3f,
	&_0x40, &_0x41, &_0x42, &_0x43, &_0x44, &_0x45, &_0x46, &_0x47, &_0x48, &_0x49, &_0x4a, &_0x4b, &_0x4c, &_0x4d, &_0x4e, &_0x4f,
 	&_0x50, &_0x51, &_0x52, &_0x53, &_0x54, &_0x55, &_0x56, &_0x57, &_0x58, &_0x59, &_0x5a, &_0x5b, &_0x5c, &_0x5d, &_0x5e, &_0x5f, 
	&_0x60, &_0x61, &_0x62, &_0x63, &_0x64, &_0x65, &_0x66, &_0x67, &_0x68, &_0x69, &_0x6a, &_0x6b, &_0x6c, &_0x6d, &_0x6e, &_0x6f, 
	&_0x70, &_0x71, &_0x72, &_0x73, &_0x74, &_0x75, &_0x76, &_0x77, &_0x78, &_0x79, &_0x7a, &_0x7b, &_0x7c, &_0x7d, &_0x7e, &_0x7f, 
	&_0x80, &_0x81, &_0x82, &_0x83, &_0x84, &_0x85, &_0x86, &_0x87, &_0x88, &_0x89, &_0x8a, &_0x8b, &_0x8c, &_0x8d, &_0x8e, &_0x8f, 
	&_0x90, &_0x91, &_0x92, &_0x93, &_0x94, &_0x95, &_0x96, &_0x97, &_0x98, &_0x99, &_0x9a, &_0x9b, &_0x9c, &_0x9d, &_0x9e, &_0x9f, 
	&_0xa0, &_0xa1, &_0xa2, &_0xa3, &_0xa4, &_0xa5, &_0xa6, &_0xa7, &_0xa8, &_0xa9, &_0xaa, &_0xab, &_0xac, &_0xad, &_0xae, &_0xaf, 
	&_0xb0, &_0xb1, &_0xb2, &_0xb3, &_0xb4, &_0xb5, &_0xb6, &_0xb7, &_0xb8, &_0xb9, &_0xba, &_0xbb, &_0xbc, &_0xbd, &_0xbe, &_0xbf, 
	&_0xc0, &_0xc1, &_0xc2, &_0xc3, &_0xc4, &_0xc5, &_0xc6, &_0xc7, &_0xc8, &_0xc9, &_0xca, &_0xcb, &_0xcc, &_0xcd, &_0xce, &_0xcf, 
	&_0xd0, &_0xd1, &_0xd2, &_0xd3, &_0xd4, &_0xd5, &_0xd6, &_0xd7, &_0xd8, &_0xd9, &_0xda, &_0xdb, &_0xdc, &_0xdd, &_0xde, &_0xdf,
	&_0xe0, &_0xe1, &_0xe2, &_0xe3, &_0xe4, &_0xe5, &_0xe6, &_0xe7, &_0xe8, &_0xe9, &_0xea, &_0xeb, &_0xec, &_0xed, &_0xee, &_0xef, 
	&_0xf0, &_0xf1, &_0xf2, &_0xf3, &_0xf4, &_0xf5, &_0xf6, &_0xf7, &_0xf8, &_0xf9, &_0xfa, &_0xfb, &_0xfc, &_0xfd, &_0xfe, &_0xff
	};
	std::copy(instructions, instructions+0x100, device->cpu.instructions);
}
