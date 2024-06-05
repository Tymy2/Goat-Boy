#include "headers/instructions.h"
#include "headers/Device.h"
#include <iostream>

#define H_SUM 1
#define H_SUB -1

#define A 1
#define F 0
#define B 3
#define C 2
#define D 5
#define E 4
#define H 7
#define L 6
#define AF 0
#define BC 1
#define DE 2
#define HL 3

#define calc_z(a) (a == 0x0)
#define calc_c8(a) (a > 0xff)
#define calc_c16(a) (a > 0xffff)

//sign indicates if it is addtition or substraction (1 or -1)
#define calc_h8(a,b,sign) ((((a & 0xf) + ( sign * (b & 0xf))) & 0x10) == 0x10)
#define calc_h16(a,b,sign) ((((a & 0xfff) + ( sign * (b & 0xfff))) & 0x1000) == 0x1000)

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
	device->mmu.write_16(device->cpu.fetch_16(), device->cpu.sp);
}

void ld_memrri_r(Device * device, uint8_t _rr_in, uint8_t _r_out){
	device->mmu.write(device->cpu.rr[_rr_in], device->cpu.r[_r_out]);
	device->cpu.rr[_rr_in]++;
}

void ld_memrrd_r(Device * device, uint8_t _rr_in, uint8_t _r_out){
	device->mmu.write(device->cpu.rr[_rr_in], device->cpu.r[_r_out]);
	device->cpu.rr[_rr_in]--;
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
	device->mmu.write(0xff00 + device->cpu.r[C], device->cpu.r[_r_out]);
}

void ld_r_memc(Device * device, uint8_t _r_in){
	device->cpu.r[_r_in] = device->mmu.read(0xff00+device->cpu.r[C]);
}

void ld_mema16_r(Device * device, uint8_t _r_out){
	device->mmu.write(device->cpu.fetch_16(), device->cpu.r[_r_out]);
}

void ld_r_mema16(Device * device, uint8_t _r_in){
	device->cpu.r[_r_in] = device->mmu.read(device->cpu.fetch_16());
}

void ld_rr_spr8(Device * device, uint8_t _rr_in){
	int8_t addr_value = int8_t(device->cpu.fetch());
	uint32_t sum = (device->cpu.sp + addr_value) & 0x1ffff;
	bool h = calc_h16(device->cpu.sp, addr_value, addr_value < 0 ? H_SUB : H_SUM );
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
	bool h = calc_h16(value_l, value_r, H_SUM);
	value_l += value_r;
	bool c = calc_c16(value_l);
	device->cpu.rr[_rr_left] = value_l;
	device->cpu.set_flags(0b0111, (h << 1) + c);
}

void add_rr_sp(Device * device, uint8_t _rr_in){
	uint32_t value_l = device->cpu.rr[_rr_in];
	bool h = calc_h16(value_l, device->cpu.sp, H_SUM);
	value_l += device->cpu.sp;
	bool c = calc_c16(value_l);
	device->cpu.rr[_rr_in] = value_l;
	device->cpu.set_flags(0b0111, (h << 1) + c);
}

void add_r_r(Device * device, uint8_t _r_in, uint8_t _r_out){
	bool h = calc_h8(device->cpu.r[_r_in], device->cpu.r[_r_out], H_SUM);
	uint16_t sum = device->cpu.r[_r_in];
	sum += device->cpu.r[_r_out];
	device->cpu.r[_r_in] = uint8_t(sum);
	bool z = calc_z(device->cpu.r[_r_in]);
	bool c = calc_c8(sum);
	device->cpu.set_flags(0b1111, (z << 3) + (h << 1) + c );
}

void add_r_memrr(Device * device, uint8_t _r_in, uint8_t _rr_out){
	uint8_t memrr = device->mmu.read(device->cpu.rr[_rr_out]);
	bool h = calc_h8(device->cpu.r[_r_in], memrr, H_SUM);
	uint16_t sum = device->cpu.r[_r_in];
	sum += memrr;
	device->cpu.r[_r_in] = uint8_t(sum);
	bool z = calc_z(device->cpu.r[_r_in]);
	bool c = calc_c8(sum);
	device->cpu.set_flags(0b1111, (z << 3) + (h << 1) + c );
}

void add_r_d8(Device * device, uint8_t _r_in){
	uint8_t addr_val = device->cpu.fetch();
	bool h = calc_h8(device->cpu.r[_r_in], addr_val, H_SUM);
	uint16_t sum = device->cpu.r[_r_in];
	sum += addr_val;
	device->cpu.r[_r_in] = uint8_t(sum);
	bool z = calc_z(device->cpu.r[_r_in]);
	bool c = calc_c8(sum);
	device->cpu.set_flags(0b1111, (z << 3) + (h << 1) + c );
}

void add_sp_r8(Device * device){
	int8_t addr_val = int8_t(device->cpu.fetch());
	bool h = calc_h16(device->cpu.sp, uint16_t(addr_val), addr_val < 0 ? H_SUB : H_SUM);
	uint32_t sum = device->cpu.sp;
	sum += addr_val;
	bool c = calc_c16(sum);
	device->cpu.sp = uint16_t(sum);
	device->cpu.set_flags(0b1111, (h << 1) + c );
}

void adc_r_r(Device * device, uint8_t _r_in, uint8_t _r_out){
	bool c_s = (device->cpu.r[F] >> 4) & 0x1;
	bool h = calc_h8(device->cpu.r[_r_in], device->cpu.r[_r_out] + c_s, H_SUM);
	uint16_t sum = device->cpu.r[_r_in];
	sum += device->cpu.r[_r_out] + c_s;
	device->cpu.r[_r_in] = uint8_t(sum);
	bool z = calc_z(device->cpu.r[_r_in]);
	bool c = calc_c8(sum);
	device->cpu.set_flags(0b1111, (z << 3) + (h << 1) + c );	
}

void adc_r_memrr(Device * device, uint8_t _r_in, uint8_t _rr_out){
	uint8_t value_memrr = device->mmu.read(device->cpu.rr[_rr_out]);
	uint8_t value_r = device->cpu.r[_r_in];
	bool carry = (device->cpu.r[F] >> 4) & 0x1;
	
	uint16_t result = value_r;
	result += value_memrr;
	result += uint8_t(carry);

	bool h = (((value_r & 0xf) + (value_memrr & 0xf) + (uint8_t(carry) & 0xf)) & 0x10) == 0x10;
	bool z = calc_z(uint8_t(result));
	bool c = calc_c8(result);

	device->cpu.r[_r_in] = uint8_t(result);
	device->cpu.set_flags(0b1111, (z << 3) + (h << 1) + c);
}

void adc_r_d8(Device * device, uint8_t _r_in){
	uint8_t value_mem = device->cpu.fetch();
	uint8_t value_r = device->cpu.r[_r_in];
	bool carry = (device->cpu.r[F] >> 4) & 0x1;
	
	uint16_t result = value_r;
	result += value_mem;
	result += uint8_t(carry);

	bool h = (((value_r & 0xf) + (value_mem & 0xf) + (uint8_t(carry) & 0xf)) & 0x10) == 0x10;
	bool z = calc_z(uint8_t(result));
	bool c = calc_c8(result);

	device->cpu.r[_r_in] = uint8_t(result);
	device->cpu.set_flags(0b1111, (z << 3) + (h << 1) + c);
}

void sub_r(Device * device, uint8_t _r_in){
	uint16_t res = device->cpu.r[A];
	res -= device->cpu.r[_r_in];
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[A], device->cpu.r[_r_in], H_SUB);
	bool c = calc_c8(res);
	device->cpu.r[A] = uint8_t(res);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void sub_memrr(Device * device, uint8_t _rr_out){
	uint16_t rr = device->cpu.rr[_rr_out];
	uint16_t res = device->cpu.r[A];
	res -= device->mmu.read(rr);
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[A], device->mmu.read(rr), H_SUB);
	bool c = calc_c8(res);
	device->cpu.r[A] = uint8_t(res);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void sub_r_d8(Device * device, uint8_t _r_in){
	uint8_t addr_val = device->cpu.fetch();
	uint16_t res = device->cpu.r[_r_in];
	res -= addr_val;
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[_r_in], addr_val, H_SUB);
	bool c = calc_c8(res);
	device->cpu.r[_r_in] = uint8_t(res);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void sbc_r_r(Device * device, uint8_t _r_in, uint8_t _r_out){
	uint16_t res = device->cpu.r[_r_in];
	bool c_s = (device->cpu.r[F] >> 4) & 0x1;
	res -= device->cpu.r[_r_out] + c_s;
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[_r_in], device->cpu.r[_r_out]+c_s, H_SUB);
	bool c = calc_c8(res);
	device->cpu.r[_r_in] = uint8_t(res);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void sbc_r_d8(Device * device, uint8_t _r_in){
	uint8_t value_mem = device->cpu.fetch();
	uint8_t value_r = device->cpu.r[_r_in];
	bool carry = (device->cpu.r[F] >> 4) & 0x1;
	
	uint16_t result = value_r;
	result -= value_mem;
	result -= uint8_t(carry);

	bool h = (((value_r & 0xf) - (value_mem & 0xf) - (uint8_t(carry) & 0xf)) & 0x10) == 0x10;
	bool z = calc_z(uint8_t(result));
	bool c = calc_c8(result);

	device->cpu.r[_r_in] = uint8_t(result);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void sbc_r_memrr(Device * device, uint8_t _r_in, uint8_t _rr_out){
	uint8_t value_memrr = device->mmu.read(device->cpu.rr[_rr_out]);
	uint8_t value_r = device->cpu.r[_r_in];
	bool carry = (device->cpu.r[F] >> 4) & 0x1;
	
	uint16_t result = value_r;
	result -= value_memrr;
	result -= uint8_t(carry);

	bool h = (((value_r & 0xf) - (value_memrr & 0xf) - (uint8_t(carry) & 0xf)) & 0x10) == 0x10;
	bool z = calc_z(uint8_t(result));
	bool c = calc_c8(result);

	device->cpu.r[_r_in] = uint8_t(result);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void inc_rr(Device * device, uint8_t _rr_in){
	device->cpu.rr[_rr_in]++;
}

void inc_memrr(Device * device, uint8_t _rr_out){
	uint16_t addr = device->cpu.rr[_rr_out];
	uint8_t value = device->mmu.read(addr);
	bool h = calc_h8(value, 1, H_SUM);
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
	bool h = calc_h8(addr_value, 1, H_SUB);
	addr_value--;
	device->mmu.write(addr, addr_value);
	bool z = calc_z(addr_value);
	device->cpu.set_flags(0b1110, (z << 3) + (0x1 << 2) + (h << 1) );
}

void inc_r(Device * device, uint8_t _r_in){
	device->cpu.r[_r_in]++;
	bool z = calc_z(device->cpu.r[_r_in]);
	bool h = calc_h8(device->cpu.r[_r_in], 1, H_SUM);
	device->cpu.set_flags(0b1110, (z << 3)+(h << 1));
}

void dec_sp(Device * device){
	device->cpu.sp--;
}

void dec_r(Device * device, uint8_t _r_in){
	bool h = calc_h8(device->cpu.r[_r_in], 1, H_SUB);
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
	if(((device->cpu.r[F] >> 7) & 0x1) == 0x0){
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
	device->cpu.set_flags(0b1111, z << 3);
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
	bool h = calc_h8(device->cpu.r[A], device->cpu.r[_r_out], H_SUB);
	bool c = calc_c8(res);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void cp_memrr(Device * device, uint8_t _rr_out){
	uint16_t rr = device->cpu.rr[_rr_out];
	uint16_t res = device->cpu.r[A];
	res -= device->mmu.read(rr);
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[A], device->mmu.read(rr), H_SUB);
	bool c = calc_c8(res);
	device->cpu.set_flags(0b1111, (z << 3) + (0x1 << 2) + (h << 1) + c);
}

void cp_d8(Device * device){
	uint8_t addr_val = device->cpu.fetch();
	uint16_t res = device->cpu.r[A];
	res -= addr_val;
	bool z = calc_z(res);
	bool h = calc_h8(device->cpu.r[A], addr_val, H_SUB);
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
	device->cpu.IME = true;
	device->cpu.pc = device->mmu.read_16(device->cpu.sp);
	device->cpu.sp += 2;
}

void pop_rr(Device * device, uint8_t _rr_in){
	device->cpu.rr[_rr_in] = device->mmu.read_16(device->cpu.sp);
	device->cpu.sp += 2;
	if(_rr_in == AF){
		device->cpu.r[F] &= 0xf0;
	}
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
	uint16_t n_pc = device->cpu.fetch_16();
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
	uint8_t cb_op = device->cpu.fetch();
	device->cpu.index_cycles = 0x100 + cb_op;
	(* device->cpu.cb_instructions[cb_op])(device);
}
	
void illegal_op(Device * device){
	std::cout << "[ERROR] this instruction is ILLEGAL -> " << int(device->mmu.read(device->cpu.pc-1)) << " PC: " << int(device->cpu.pc) << std::endl;
	device->keep_running = false;
}

void di(Device * device){
	device->cpu.IME = false;
}

void ei(Device * device){
	device->cpu.IME = true;
}

void daa(Device * device){
	//credits: https://forums.nesdev.org/viewtopic.php?t=15944
	bool n_flag = (device->cpu.r[F] >> 6) & 1;
	bool c_flag = (device->cpu.r[F] >> 4) & 1;
	bool h_flag = (device->cpu.r[F] >> 5) & 1;
	bool z_flag = (device->cpu.r[F] >> 7) & 1;
	uint8_t a = device->cpu.r[A];
	if (!n_flag) {
		if (c_flag || a > 0x99) { a += 0x60; c_flag = 1; }
  		if (h_flag || (a & 0x0f) > 0x09) { a += 0x6; }
	} else {  
		if (c_flag) { a -= 0x60; }
  		if (h_flag) { a -= 0x6; }
	}
	z_flag = calc_z(a);
	device->cpu.r[A] = a;
	device->cpu.set_flags(0b1011, (z_flag << 3) + c_flag);
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

//CB INSTRUCTIONS
//TODO some instructions are not correct, dont know which implementation is the correct one, requires testing in future

void rlc_r(Device * device, uint8_t _r){
	uint8_t value = device->cpu.r[_r];
	uint8_t bit = value >> 7;
	value = (value << 1) | bit;
	bool z = calc_z(value);
	device->cpu.r[_r] = value;
	device->cpu.set_flags(0b1111, (z << 3) + bit);
}

void rlc_memrr(Device * device, uint8_t _rr){
	uint16_t addr = device->cpu.rr[_rr];
	uint8_t value = device->mmu.read(addr);
	uint8_t bit = value >> 7;
	value = (value << 1) | bit;
	bool z = calc_z(value);
	device->mmu.write(addr, value);
	device->cpu.set_flags(0b1111, (z << 3) + bit);
}

void rrc_r(Device * device, uint8_t _r){
	uint8_t value = device->cpu.r[_r];
	uint8_t bit = value & 0x1;
	value = (value >> 1) | (bit << 7);
	bool z = calc_z(value);
	device->cpu.r[_r] = value;
	device->cpu.set_flags(0b1111, (z << 3) + bit);
}

void rrc_memrr(Device * device, uint8_t _rr){
	uint16_t addr = device->cpu.rr[_rr];
	uint8_t value = device->mmu.read(addr);
	uint8_t bit = value & 0x1;
	value = (value >> 1) | (bit << 7);
	bool z = calc_z(value);
	device->mmu.write(addr, value);
	device->cpu.set_flags(0b1111, (z << 3) + bit);
}

void rl_r(Device * device, uint8_t _r){
	bool bit = device->cpu.r[_r] >> 7;
	device->cpu.r[_r] = ((device->cpu.r[_r] << 1) & 0xff) + ((device->cpu.r[F] >> 4) & 0x1);
	bool z = calc_z(device->cpu.r[_r]);
	device->cpu.set_flags(0b1111, (z << 3) + bit);
}

void rl_memrr(Device * device, uint8_t _rr){
	uint16_t addr = device->cpu.rr[_rr];
	uint8_t value = device->mmu.read(addr);
	bool bit = value >> 7;
	value = ((value << 1) & 0xff) + ((device->cpu.r[F] >> 4) & 0x1 );
	bool z = calc_z(value);
	device->mmu.write(addr, value);
	device->cpu.set_flags(0b1111, (z << 3) + bit);
}

void rr_r(Device * device, uint8_t _r){
	bool bit = device->cpu.r[_r] & 0x1;
	device->cpu.r[_r] = ((device->cpu.r[_r] >> 1) & 0xff) + (((device->cpu.r[F] >> 4) & 0x1) << 7);
	bool z = calc_z(device->cpu.r[_r]);
	device->cpu.set_flags(0b1111, (z << 3) + bit);
}

void rr_memrr(Device * device, uint8_t _rr){
	uint16_t addr = device->cpu.rr[_rr];
	uint8_t value = device->mmu.read(addr);
	bool bit = value & 0x1;
	value = ((value >> 1) & 0xff) + (((device->cpu.r[F] >> 4) & 0x1) << 7);
	bool z = calc_z(value);
	device->mmu.write(addr, value);
	device->cpu.set_flags(0b1111, (z << 3) + bit);
}

void sla_r(Device * device, uint8_t _r){
	uint8_t value = device->cpu.r[_r];
	uint8_t bit = value >> 7;
	device->cpu.r[_r] <<= 1;
	bool z = calc_z(device->cpu.r[_r]);
	device->cpu.set_flags(0b1111, (z << 3) + bit);
}

void sla_memrr(Device * device, uint8_t _rr){
	uint16_t addr = device->cpu.rr[_rr];
	uint8_t value = device->mmu.read(addr);
	bool bit = value >> 7;
	value <<= 1;
	device->mmu.write(addr, value);	
	bool z = calc_z(value);
	device->cpu.set_flags(0b1111, (z << 3) + bit);
}

void sra_r(Device * device, uint8_t _r){
	uint8_t value = device->cpu.r[_r];
	uint8_t bit_7 = value & 0x80; // get bit at pos 7
	value >>= 1;
	value |= bit_7;
	device->cpu.r[_r] = value;
	bool z = calc_z(value);
	device->cpu.set_flags(0b1111, z << 3);
}

void sra_memrr(Device * device, uint8_t _rr){
	uint16_t addr = device->cpu.rr[_rr];
	uint8_t value = device->mmu.read(addr);
	bool bit = value & 0x1;
	uint8_t bit_7 = value & 0x80; // get bit at pos 7
	value >>= 1;
	value |= bit_7;
	device->mmu.write(addr, value);
	bool z = calc_z(value);
	device->cpu.set_flags(0b1111, (z << 3) + bit);
}

void srl_r(Device * device, uint8_t _r){
	uint8_t value = device->cpu.r[_r];
	bool bit = value & 0x1;
	value >>= 1;
	bool z = calc_z(value);
	device->cpu.r[_r] = value;
	device->cpu.set_flags(0b1111, (z << 3) + bit);
}

void srl_memrr(Device * device, uint8_t _rr){
	uint16_t addr = device->cpu.rr[_rr];
	uint8_t value = device->mmu.read(addr);
	bool bit = value & 0x1;
	value >>= 1;
	bool z = calc_z(value);
	device->mmu.write(addr, value);
	device->cpu.set_flags(0b1111, (z << 3) + bit);
}

void bit_n_r(Device * device, uint8_t bit_pos, uint8_t _r){
	uint8_t value = device->cpu.r[_r];
	bool bit = ((value >> bit_pos) & 0x1) ^ 0x1;
	device->cpu.set_flags(0b1110, (bit << 3) + (0x1 << 1));
}

void bit_n_memrr(Device * device, uint8_t bit_pos, uint8_t _rr){
	uint8_t value = device->mmu.read(device->cpu.rr[_rr]);
	bool bit = ((value >> (bit_pos)) & 0x1) ^ 0x1;
	device->cpu.set_flags(0b1110, (bit << 3) + (0x1 << 1));
}

void res_n_r(Device * device, uint8_t bit_pos, uint8_t _r){
	device->cpu.r[_r] &= (0x1 << bit_pos) ^ 0xff;
}

void res_n_memrr(Device * device, uint8_t bit_pos, uint8_t _rr){
	uint16_t addr = device->cpu.rr[_rr];
	uint8_t value = device->mmu.read(addr);
	value &= (0x1 << bit_pos) ^ 0xff;
	device->mmu.write(addr, value);
}

void set_n_r(Device * device, uint8_t bit_pos, uint8_t _r){
	device->cpu.r[_r] |= 0x1 << bit_pos;
}

void set_n_memrr(Device * device, uint8_t bit_pos, uint8_t _rr){
	uint16_t addr = device->cpu.rr[_rr];
	uint8_t value = device->mmu.read(addr);
	value |= 0x1 << bit_pos;
	device->mmu.write(addr, value);
}

void swap_r(Device * device, uint8_t _r){
	uint8_t value = device->cpu.r[_r];
	uint8_t left_nibble = (value >> 4);
	uint8_t right_nibble = (value & 0xf) << 4;
	value = left_nibble + right_nibble;
	device->cpu.r[_r] = value;
	bool z = calc_z(value);
	device->cpu.set_flags(0b1111, z << 3);
}

void swap_memrr(Device * device, uint8_t _rr){
	uint16_t addr = device->cpu.rr[_rr];
	uint8_t value = device->mmu.read(addr);
	uint8_t left_nibble = (value >> 4);
	uint8_t right_nibble = (value & 0xf) << 4;
	value = left_nibble + right_nibble;
	device->mmu.write(addr, value);
	bool z = calc_z(value);
	device->cpu.set_flags(0b1111, z << 3);
}


void cb_0x00(Device * device){ rlc_r(device, B); }
void cb_0x01(Device * device){ rlc_r(device, C); }
void cb_0x02(Device * device){ rlc_r(device, D); }
void cb_0x03(Device * device){ rlc_r(device, E); }
void cb_0x04(Device * device){ rlc_r(device, H); }
void cb_0x05(Device * device){ rlc_r(device, L); }
void cb_0x06(Device * device){ rlc_memrr(device, HL); }
void cb_0x07(Device * device){ rlc_r(device, A); }
void cb_0x08(Device * device){ rrc_r(device, B); }
void cb_0x09(Device * device){ rrc_r(device, C); }
void cb_0x0a(Device * device){ rrc_r(device, D); }
void cb_0x0b(Device * device){ rrc_r(device, E); }
void cb_0x0c(Device * device){ rrc_r(device, H); }
void cb_0x0d(Device * device){ rrc_r(device, L); }
void cb_0x0e(Device * device){ rrc_memrr(device, HL); }
void cb_0x0f(Device * device){ rrc_r(device, A); }
void cb_0x10(Device * device){ rl_r(device, B); }
void cb_0x11(Device * device){ rl_r(device, C); }
void cb_0x12(Device * device){ rl_r(device, D); }
void cb_0x13(Device * device){ rl_r(device, E); }
void cb_0x14(Device * device){ rl_r(device, H); }
void cb_0x15(Device * device){ rl_r(device, L); }
void cb_0x16(Device * device){ rl_memrr(device, HL); }
void cb_0x17(Device * device){ rl_r(device, A); }
void cb_0x18(Device * device){ rr_r(device, B); }
void cb_0x19(Device * device){ rr_r(device, C); }
void cb_0x1a(Device * device){ rr_r(device, D); }
void cb_0x1b(Device * device){ rr_r(device, E); }
void cb_0x1c(Device * device){ rr_r(device, H); }
void cb_0x1d(Device * device){ rr_r(device, L); }
void cb_0x1e(Device * device){ rr_memrr(device, HL); }
void cb_0x1f(Device * device){ rr_r(device, A); }
void cb_0x20(Device * device){ sla_r(device, B); }
void cb_0x21(Device * device){ sla_r(device, C); }
void cb_0x22(Device * device){ sla_r(device, D); }
void cb_0x23(Device * device){ sla_r(device, E); }
void cb_0x24(Device * device){ sla_r(device, H); }
void cb_0x25(Device * device){ sla_r(device, L); }
void cb_0x26(Device * device){ sla_memrr(device, HL); }
void cb_0x27(Device * device){ sla_r(device, A); }
void cb_0x28(Device * device){ sra_r(device, B); }
void cb_0x29(Device * device){ sra_r(device, C); }
void cb_0x2a(Device * device){ sra_r(device, D); }
void cb_0x2b(Device * device){ sra_r(device, E); }
void cb_0x2c(Device * device){ sra_r(device, H); }
void cb_0x2d(Device * device){ sra_r(device, L); }
void cb_0x2e(Device * device){ sra_memrr(device, HL); }
void cb_0x2f(Device * device){ sra_r(device, A); }
void cb_0x30(Device * device){ swap_r(device, B); }
void cb_0x31(Device * device){ swap_r(device, C); }
void cb_0x32(Device * device){ swap_r(device, D); }
void cb_0x33(Device * device){ swap_r(device, E); }
void cb_0x34(Device * device){ swap_r(device, H); }
void cb_0x35(Device * device){ swap_r(device, L); }
void cb_0x36(Device * device){ swap_memrr(device, HL); }
void cb_0x37(Device * device){ swap_r(device, A); }
void cb_0x38(Device * device){ srl_r(device, B); }
void cb_0x39(Device * device){ srl_r(device, C); }
void cb_0x3a(Device * device){ srl_r(device, D); }
void cb_0x3b(Device * device){ srl_r(device, E); }
void cb_0x3c(Device * device){ srl_r(device, H); }
void cb_0x3d(Device * device){ srl_r(device, L); }
void cb_0x3e(Device * device){ srl_memrr(device, HL); }
void cb_0x3f(Device * device){ srl_r(device, A); }
void cb_0x40(Device * device){ bit_n_r(device, 0, B); }
void cb_0x41(Device * device){ bit_n_r(device, 0, C); }
void cb_0x42(Device * device){ bit_n_r(device, 0, D); }
void cb_0x43(Device * device){ bit_n_r(device, 0, E); }
void cb_0x44(Device * device){ bit_n_r(device, 0, H); }
void cb_0x45(Device * device){ bit_n_r(device, 0, L); }
void cb_0x46(Device * device){ bit_n_memrr(device, 0, HL); }
void cb_0x47(Device * device){ bit_n_r(device, 0, A); }
void cb_0x48(Device * device){ bit_n_r(device, 1, B); }
void cb_0x49(Device * device){ bit_n_r(device, 1, C); }
void cb_0x4a(Device * device){ bit_n_r(device, 1, D); }
void cb_0x4b(Device * device){ bit_n_r(device, 1, E); }
void cb_0x4c(Device * device){ bit_n_r(device, 1, H); }
void cb_0x4d(Device * device){ bit_n_r(device, 1, L); }
void cb_0x4e(Device * device){ bit_n_memrr(device, 1, HL); }
void cb_0x4f(Device * device){ bit_n_r(device, 1, A); }
void cb_0x50(Device * device){ bit_n_r(device, 2, B); }
void cb_0x51(Device * device){ bit_n_r(device, 2, C); }
void cb_0x52(Device * device){ bit_n_r(device, 2, D); }
void cb_0x53(Device * device){ bit_n_r(device, 2, E); }
void cb_0x54(Device * device){ bit_n_r(device, 2, H); }
void cb_0x55(Device * device){ bit_n_r(device, 2, L); }
void cb_0x56(Device * device){ bit_n_memrr(device, 2, HL); }
void cb_0x57(Device * device){ bit_n_r(device, 2, A); }
void cb_0x58(Device * device){ bit_n_r(device, 3, B); }
void cb_0x59(Device * device){ bit_n_r(device, 3, C); }
void cb_0x5a(Device * device){ bit_n_r(device, 3, D); }
void cb_0x5b(Device * device){ bit_n_r(device, 3, E); }
void cb_0x5c(Device * device){ bit_n_r(device, 3, H); }
void cb_0x5d(Device * device){ bit_n_r(device, 3, L); }
void cb_0x5e(Device * device){ bit_n_memrr(device, 3, HL); }
void cb_0x5f(Device * device){ bit_n_r(device, 3, A); }
void cb_0x60(Device * device){ bit_n_r(device, 4, B); }
void cb_0x61(Device * device){ bit_n_r(device, 4, C); }
void cb_0x62(Device * device){ bit_n_r(device, 4, D); }
void cb_0x63(Device * device){ bit_n_r(device, 4, E); }
void cb_0x64(Device * device){ bit_n_r(device, 4, H); }
void cb_0x65(Device * device){ bit_n_r(device, 4, L); }
void cb_0x66(Device * device){ bit_n_memrr(device, 4, HL); }
void cb_0x67(Device * device){ bit_n_r(device, 4, A); }
void cb_0x68(Device * device){ bit_n_r(device, 5, B); }
void cb_0x69(Device * device){ bit_n_r(device, 5, C); }
void cb_0x6a(Device * device){ bit_n_r(device, 5, D); }
void cb_0x6b(Device * device){ bit_n_r(device, 5, E); }
void cb_0x6c(Device * device){ bit_n_r(device, 5, H); }
void cb_0x6d(Device * device){ bit_n_r(device, 5, L); }
void cb_0x6e(Device * device){ bit_n_memrr(device, 5, HL); }
void cb_0x6f(Device * device){ bit_n_r(device, 5, A); }
void cb_0x70(Device * device){ bit_n_r(device, 6, B); }
void cb_0x71(Device * device){ bit_n_r(device, 6, C); }
void cb_0x72(Device * device){ bit_n_r(device, 6, D); }
void cb_0x73(Device * device){ bit_n_r(device, 6, E); }
void cb_0x74(Device * device){ bit_n_r(device, 6, H); }
void cb_0x75(Device * device){ bit_n_r(device, 6, L); }
void cb_0x76(Device * device){ bit_n_memrr(device, 6, HL); }
void cb_0x77(Device * device){ bit_n_r(device, 6, A); }
void cb_0x78(Device * device){ bit_n_r(device, 7, B); }
void cb_0x79(Device * device){ bit_n_r(device, 7, C); }
void cb_0x7a(Device * device){ bit_n_r(device, 7, D); }
void cb_0x7b(Device * device){ bit_n_r(device, 7, E); }
void cb_0x7c(Device * device){ bit_n_r(device, 7, H); }
void cb_0x7d(Device * device){ bit_n_r(device, 7, L); }
void cb_0x7e(Device * device){ bit_n_memrr(device, 7, HL); }
void cb_0x7f(Device * device){ bit_n_r(device, 7, A); }
void cb_0x80(Device * device){ res_n_r(device, 0, B); }
void cb_0x81(Device * device){ res_n_r(device, 0, C); }
void cb_0x82(Device * device){ res_n_r(device, 0, D); }
void cb_0x83(Device * device){ res_n_r(device, 0, E); }
void cb_0x84(Device * device){ res_n_r(device, 0, H); }
void cb_0x85(Device * device){ res_n_r(device, 0, L); }
void cb_0x86(Device * device){ res_n_memrr(device, 0, HL); }
void cb_0x87(Device * device){ res_n_r(device, 0, A); }
void cb_0x88(Device * device){ res_n_r(device, 1, B); }
void cb_0x89(Device * device){ res_n_r(device, 1, C); }
void cb_0x8a(Device * device){ res_n_r(device, 1, D); }
void cb_0x8b(Device * device){ res_n_r(device, 1, E); }
void cb_0x8c(Device * device){ res_n_r(device, 1, H); }
void cb_0x8d(Device * device){ res_n_r(device, 1, L); }
void cb_0x8e(Device * device){ res_n_memrr(device, 1, HL); }
void cb_0x8f(Device * device){ res_n_r(device, 1, A); }
void cb_0x90(Device * device){ res_n_r(device, 2, B); }
void cb_0x91(Device * device){ res_n_r(device, 2, C); }
void cb_0x92(Device * device){ res_n_r(device, 2, D); }
void cb_0x93(Device * device){ res_n_r(device, 2, E); }
void cb_0x94(Device * device){ res_n_r(device, 2, H); }
void cb_0x95(Device * device){ res_n_r(device, 2, L); }
void cb_0x96(Device * device){ res_n_memrr(device, 2, HL); }
void cb_0x97(Device * device){ res_n_r(device, 2, A); }
void cb_0x98(Device * device){ res_n_r(device, 3, B); }
void cb_0x99(Device * device){ res_n_r(device, 3, C); }
void cb_0x9a(Device * device){ res_n_r(device, 3, D); }
void cb_0x9b(Device * device){ res_n_r(device, 3, E); }
void cb_0x9c(Device * device){ res_n_r(device, 3, H); }
void cb_0x9d(Device * device){ res_n_r(device, 3, L); }
void cb_0x9e(Device * device){ res_n_memrr(device, 3, HL); }
void cb_0x9f(Device * device){ res_n_r(device, 3, A); }
void cb_0xa0(Device * device){ res_n_r(device, 4, B); }
void cb_0xa1(Device * device){ res_n_r(device, 4, C); }
void cb_0xa2(Device * device){ res_n_r(device, 4, D); }
void cb_0xa3(Device * device){ res_n_r(device, 4, E); }
void cb_0xa4(Device * device){ res_n_r(device, 4, H); }
void cb_0xa5(Device * device){ res_n_r(device, 4, L); }
void cb_0xa6(Device * device){ res_n_memrr(device, 4, HL); }
void cb_0xa7(Device * device){ res_n_r(device, 4, A); }
void cb_0xa8(Device * device){ res_n_r(device, 5, B); }
void cb_0xa9(Device * device){ res_n_r(device, 5, C); }
void cb_0xaa(Device * device){ res_n_r(device, 5, D); }
void cb_0xab(Device * device){ res_n_r(device, 5, E); }
void cb_0xac(Device * device){ res_n_r(device, 5, H); }
void cb_0xad(Device * device){ res_n_r(device, 5, L); }
void cb_0xae(Device * device){ res_n_memrr(device, 5, HL); }
void cb_0xaf(Device * device){ res_n_r(device, 5, A); }
void cb_0xb0(Device * device){ res_n_r(device, 6, B); }
void cb_0xb1(Device * device){ res_n_r(device, 6, C); }
void cb_0xb2(Device * device){ res_n_r(device, 6, D); }
void cb_0xb3(Device * device){ res_n_r(device, 6, E); }
void cb_0xb4(Device * device){ res_n_r(device, 6, H); }
void cb_0xb5(Device * device){ res_n_r(device, 6, L); }
void cb_0xb6(Device * device){ res_n_memrr(device, 6, HL); }
void cb_0xb7(Device * device){ res_n_r(device, 6, A); }
void cb_0xb8(Device * device){ res_n_r(device, 7, B); }
void cb_0xb9(Device * device){ res_n_r(device, 7, C); }
void cb_0xba(Device * device){ res_n_r(device, 7, D); }
void cb_0xbb(Device * device){ res_n_r(device, 7, E); }
void cb_0xbc(Device * device){ res_n_r(device, 7, H); }
void cb_0xbd(Device * device){ res_n_r(device, 7, L); }
void cb_0xbe(Device * device){ res_n_memrr(device, 7, HL); }
void cb_0xbf(Device * device){ res_n_r(device, 7, A); }
void cb_0xc0(Device * device){ set_n_r(device, 0, B); }
void cb_0xc1(Device * device){ set_n_r(device, 0, C); }
void cb_0xc2(Device * device){ set_n_r(device, 0, D); }
void cb_0xc3(Device * device){ set_n_r(device, 0, E); }
void cb_0xc4(Device * device){ set_n_r(device, 0, H); }
void cb_0xc5(Device * device){ set_n_r(device, 0, L); }
void cb_0xc6(Device * device){ set_n_memrr(device, 0, HL); }
void cb_0xc7(Device * device){ set_n_r(device, 0, A); }
void cb_0xc8(Device * device){ set_n_r(device, 1, B); }
void cb_0xc9(Device * device){ set_n_r(device, 1, C); }
void cb_0xca(Device * device){ set_n_r(device, 1, D); }
void cb_0xcb(Device * device){ set_n_r(device, 1, E); }
void cb_0xcc(Device * device){ set_n_r(device, 1, H); }
void cb_0xcd(Device * device){ set_n_r(device, 1, L); }
void cb_0xce(Device * device){ set_n_memrr(device, 1, HL); }
void cb_0xcf(Device * device){ set_n_r(device, 1, A); }
void cb_0xd0(Device * device){ set_n_r(device, 2, B); }
void cb_0xd1(Device * device){ set_n_r(device, 2, C); }
void cb_0xd2(Device * device){ set_n_r(device, 2, D); }
void cb_0xd3(Device * device){ set_n_r(device, 2, E); }
void cb_0xd4(Device * device){ set_n_r(device, 2, H); }
void cb_0xd5(Device * device){ set_n_r(device, 2, L); }
void cb_0xd6(Device * device){ set_n_memrr(device, 2, HL); }
void cb_0xd7(Device * device){ set_n_r(device, 2, A); }
void cb_0xd8(Device * device){ set_n_r(device, 3, B); }
void cb_0xd9(Device * device){ set_n_r(device, 3, C); }
void cb_0xda(Device * device){ set_n_r(device, 3, D); }
void cb_0xdb(Device * device){ set_n_r(device, 3, E); }
void cb_0xdc(Device * device){ set_n_r(device, 3, H); }
void cb_0xdd(Device * device){ set_n_r(device, 3, L); }
void cb_0xde(Device * device){ set_n_memrr(device, 3, HL); }
void cb_0xdf(Device * device){ set_n_r(device, 3, A); }
void cb_0xe0(Device * device){ set_n_r(device, 4, B); }
void cb_0xe1(Device * device){ set_n_r(device, 4, C); }
void cb_0xe2(Device * device){ set_n_r(device, 4, D); }
void cb_0xe3(Device * device){ set_n_r(device, 4, E); }
void cb_0xe4(Device * device){ set_n_r(device, 4, H); }
void cb_0xe5(Device * device){ set_n_r(device, 4, L); }
void cb_0xe6(Device * device){ set_n_memrr(device, 4, HL); }
void cb_0xe7(Device * device){ set_n_r(device, 4, A); }
void cb_0xe8(Device * device){ set_n_r(device, 5, B); }
void cb_0xe9(Device * device){ set_n_r(device, 5, C); }
void cb_0xea(Device * device){ set_n_r(device, 5, D); }
void cb_0xeb(Device * device){ set_n_r(device, 5, E); }
void cb_0xec(Device * device){ set_n_r(device, 5, H); }
void cb_0xed(Device * device){ set_n_r(device, 5, L); }
void cb_0xee(Device * device){ set_n_memrr(device, 5, HL); }
void cb_0xef(Device * device){ set_n_r(device, 5, A); }
void cb_0xf0(Device * device){ set_n_r(device, 6, B); }
void cb_0xf1(Device * device){ set_n_r(device, 6, C); }
void cb_0xf2(Device * device){ set_n_r(device, 6, D); }
void cb_0xf3(Device * device){ set_n_r(device, 6, E); }
void cb_0xf4(Device * device){ set_n_r(device, 6, H); }
void cb_0xf5(Device * device){ set_n_r(device, 6, L); }
void cb_0xf6(Device * device){ set_n_memrr(device, 6, HL); }
void cb_0xf7(Device * device){ set_n_r(device, 6, A); }
void cb_0xf8(Device * device){ set_n_r(device, 7, B); }
void cb_0xf9(Device * device){ set_n_r(device, 7, C); }
void cb_0xfa(Device * device){ set_n_r(device, 7, D); }
void cb_0xfb(Device * device){ set_n_r(device, 7, E); }
void cb_0xfc(Device * device){ set_n_r(device, 7, H); }
void cb_0xfd(Device * device){ set_n_r(device, 7, L); }
void cb_0xfe(Device * device){ set_n_memrr(device, 7, HL); }
void cb_0xff(Device * device){ set_n_r(device, 7, A); }

void setup_cb_instructions(Device * device){
	void (* cb_instructions[0x100])(Device *) = {
	&cb_0x00, &cb_0x01, &cb_0x02, &cb_0x03, &cb_0x04, &cb_0x05, &cb_0x06, &cb_0x07, &cb_0x08, &cb_0x09, &cb_0x0a, &cb_0x0b, &cb_0x0c, &cb_0x0d, &cb_0x0e, &cb_0x0f,
	&cb_0x10, &cb_0x11, &cb_0x12, &cb_0x13, &cb_0x14, &cb_0x15, &cb_0x16, &cb_0x17, &cb_0x18, &cb_0x19, &cb_0x1a, &cb_0x1b, &cb_0x1c, &cb_0x1d, &cb_0x1e, &cb_0x1f,
	&cb_0x20, &cb_0x21, &cb_0x22, &cb_0x23, &cb_0x24, &cb_0x25, &cb_0x26, &cb_0x27, &cb_0x28, &cb_0x29, &cb_0x2a, &cb_0x2b, &cb_0x2c, &cb_0x2d, &cb_0x2e, &cb_0x2f,
 	&cb_0x30, &cb_0x31, &cb_0x32, &cb_0x33, &cb_0x34, &cb_0x35, &cb_0x36, &cb_0x37, &cb_0x38, &cb_0x39, &cb_0x3a, &cb_0x3b, &cb_0x3c, &cb_0x3d, &cb_0x3e, &cb_0x3f,
	&cb_0x40, &cb_0x41, &cb_0x42, &cb_0x43, &cb_0x44, &cb_0x45, &cb_0x46, &cb_0x47, &cb_0x48, &cb_0x49, &cb_0x4a, &cb_0x4b, &cb_0x4c, &cb_0x4d, &cb_0x4e, &cb_0x4f,
 	&cb_0x50, &cb_0x51, &cb_0x52, &cb_0x53, &cb_0x54, &cb_0x55, &cb_0x56, &cb_0x57, &cb_0x58, &cb_0x59, &cb_0x5a, &cb_0x5b, &cb_0x5c, &cb_0x5d, &cb_0x5e, &cb_0x5f, 
	&cb_0x60, &cb_0x61, &cb_0x62, &cb_0x63, &cb_0x64, &cb_0x65, &cb_0x66, &cb_0x67, &cb_0x68, &cb_0x69, &cb_0x6a, &cb_0x6b, &cb_0x6c, &cb_0x6d, &cb_0x6e, &cb_0x6f, 
	&cb_0x70, &cb_0x71, &cb_0x72, &cb_0x73, &cb_0x74, &cb_0x75, &cb_0x76, &cb_0x77, &cb_0x78, &cb_0x79, &cb_0x7a, &cb_0x7b, &cb_0x7c, &cb_0x7d, &cb_0x7e, &cb_0x7f, 
	&cb_0x80, &cb_0x81, &cb_0x82, &cb_0x83, &cb_0x84, &cb_0x85, &cb_0x86, &cb_0x87, &cb_0x88, &cb_0x89, &cb_0x8a, &cb_0x8b, &cb_0x8c, &cb_0x8d, &cb_0x8e, &cb_0x8f, 
	&cb_0x90, &cb_0x91, &cb_0x92, &cb_0x93, &cb_0x94, &cb_0x95, &cb_0x96, &cb_0x97, &cb_0x98, &cb_0x99, &cb_0x9a, &cb_0x9b, &cb_0x9c, &cb_0x9d, &cb_0x9e, &cb_0x9f, 
	&cb_0xa0, &cb_0xa1, &cb_0xa2, &cb_0xa3, &cb_0xa4, &cb_0xa5, &cb_0xa6, &cb_0xa7, &cb_0xa8, &cb_0xa9, &cb_0xaa, &cb_0xab, &cb_0xac, &cb_0xad, &cb_0xae, &cb_0xaf, 
	&cb_0xb0, &cb_0xb1, &cb_0xb2, &cb_0xb3, &cb_0xb4, &cb_0xb5, &cb_0xb6, &cb_0xb7, &cb_0xb8, &cb_0xb9, &cb_0xba, &cb_0xbb, &cb_0xbc, &cb_0xbd, &cb_0xbe, &cb_0xbf, 
	&cb_0xc0, &cb_0xc1, &cb_0xc2, &cb_0xc3, &cb_0xc4, &cb_0xc5, &cb_0xc6, &cb_0xc7, &cb_0xc8, &cb_0xc9, &cb_0xca, &cb_0xcb, &cb_0xcc, &cb_0xcd, &cb_0xce, &cb_0xcf, 
	&cb_0xd0, &cb_0xd1, &cb_0xd2, &cb_0xd3, &cb_0xd4, &cb_0xd5, &cb_0xd6, &cb_0xd7, &cb_0xd8, &cb_0xd9, &cb_0xda, &cb_0xdb, &cb_0xdc, &cb_0xdd, &cb_0xde, &cb_0xdf,
	&cb_0xe0, &cb_0xe1, &cb_0xe2, &cb_0xe3, &cb_0xe4, &cb_0xe5, &cb_0xe6, &cb_0xe7, &cb_0xe8, &cb_0xe9, &cb_0xea, &cb_0xeb, &cb_0xec, &cb_0xed, &cb_0xee, &cb_0xef, 
	&cb_0xf0, &cb_0xf1, &cb_0xf2, &cb_0xf3, &cb_0xf4, &cb_0xf5, &cb_0xf6, &cb_0xf7, &cb_0xf8, &cb_0xf9, &cb_0xfa, &cb_0xfb, &cb_0xfc, &cb_0xfd, &cb_0xfe, &cb_0xff
	};
	std::copy(cb_instructions, cb_instructions+0x100, device->cpu.cb_instructions);
}
