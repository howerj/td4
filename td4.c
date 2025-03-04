#define TD4_PROJECT "A simulator for 4-bit CPU originally from https://github.com/wuxx/TD4-4BIT-CPU"
#define TD4_AUTHOR "Richard James Howe" /* author of this simulator/assembler, not the original 4-bit CPU */
#define TD4_EMAIL "howe.r.j.89@gmail.com"
#define TD4_REPO "https://github.com/howerj/td4"
#define TD4_LICENSE "0BSD"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
	uint8_t a, b, in, out, carry;
	uint8_t pc, rom[16];
} td4_t;

static int td4_step(td4_t *t, FILE *in, FILE *out) {
	assert(t);
	const uint8_t ins = t->rom[t->pc & 0xF];
	const uint8_t alu = (ins >> 4) & 0xF;
	const uint8_t imm = (ins >> 0) & 0xF;
	uint8_t next = t->pc + 1;
	if (in) {
		if (alu == 2 || alu == 7) {
			unsigned d = 0;
			if (out) {
				if (fprintf(out, "IN: ") < 0) return -1;
			}
			if (fscanf(in, "%x", &d) != 1) return -1;
			t->in = d & 0xF;
		}
	}
	switch (alu) {
	case 0x0: t->a += imm; t->carry = !!(t->a & 0xF0); break;
	case 0x1: t->a = t->b; break;
	case 0x2: t->a = t->in; break;
	case 0x3: t->a = imm; break;

	case 0x4: t->b = t->a; break;
	case 0x5: t->b = imm; break;
	case 0x6: t->b += imm; t->carry = !!(t->b & 0xF0); break;
	case 0x7: t->b = t->in; break;

	case 0x8: break;
	case 0x9: t->out = t->b; break;
	case 0xA: t->out = imm; break;
	case 0xB: break;

	case 0xC: break;
	case 0xD: break;
	case 0xE: next = t->carry ? imm : next; break;
	case 0xF: next = imm; break;
	}
	t->a &= 0xF;
	t->b &= 0xF;
	if (out) {
		static const char *name[] = {
			"ADD A,Im", "MOV A,B ", "IN A    ", "MOV A,Im",
			"MOV B,  ", "MOV B,Im", "ADD B,Im", "IN B    ",
			"???     ", "OUT B   ", "OUT Im  ", "???     ",
			"???     ", "???     ", "JC Im   ", "JMP Im  ",
		};
		if (fprintf(out, 
			"PC:%X %s IM:%X A:%X B:%X OUT:%X CARRY:%X\n", 
			t->pc, name[alu], imm, t->a, t->b, t->out, t->carry) < 0) return -1;
	}
	t->pc = next & 0xF;
	return 0;
}

static void lowerify(char *s, size_t len) {
	for (size_t i = 0; i < len; i++) {
		const int ch  = s[i];
		if (ch >= 'A' && ch <= 'Z')
			s[i] = ch ^ 32;
	}
}

static int numerify(const char *n) {
	char *end = NULL;
	const int e = errno;
	errno = 0;
	const long r = strtol(n, &end, 0);
	if (errno || !end || *end) {
		errno = e;
		return -1;
	}
	errno = e;
	return r & 0xFl;
}

static int td4_asm(td4_t *t, FILE *in, FILE *err) {
	assert(t);
	assert(in);
	assert(err);
	int s = 0, line = 1;
	memset(t, 0, sizeof (*t));
	struct hole { int used, pc, val; } holes[16] = { { .pc = 0, }, };
	struct labels {
		char label[8];
		int pos;
	} lables[16] = {
		{ .label = { 0, }, },
	};
	for (s = 0; s < 17; line++) {
		char buf[256] = { 0, }, op[8] = { 0,}, arg1[8] = { 0, }, arg2[8] = { 0, };
		int args = 0;
		lowerify(op, sizeof (op));
		lowerify(arg1, sizeof(arg1));
		lowerify(arg2, sizeof(arg2));
		if (!fgets(buf, sizeof(buf), in))
			break;
		for (size_t i = 0; i < sizeof(buf); i++) {
			const int ch = buf[i];
			if (ch == ';' || ch == '#')
				buf[i] = '\0';
		}
		args = sscanf(buf, "%7s %7s %7s", op, arg1, arg2);
		if (args <= 0) {
			continue;
		} else if (!strcmp(op, "add")) {
			if (args != 3) { return -1; }
			if (!strcmp(arg1, "a")) {
				t->rom[s++ % 16] = (0x0 << 4) | (atoi(arg2) & 0xF);
			} else if (!strcmp(arg1, "b")) {
				t->rom[s++ % 16] = (0x6 << 4) | (atoi(arg2) & 0xF);
			} else {
				(void)fprintf(stderr, "invalid operands to add on line %d\n", line);
				return -1;
			}
		} else if (!strcmp(op, "mov")) {
			if (args != 3) { return -1; }
			if (!strcmp(arg1, "a")) {
				if (!strcmp(arg2, "b")) {
					t->rom[s++ % 16] = (0x1 << 4);
				} else {
					t->rom[s++ % 16] = (0x3 << 4) | (atoi(arg2) & 0xF);
				}
			} if (!strcmp(arg1, "b")) {
				if (!strcmp(arg2, "a")) {
					t->rom[s++ % 16] = (0x4 << 4);
				} else {
					t->rom[s++ % 16] = (0x5 << 4) | (atoi(arg2) & 0xF);
				}
			} else {
				(void)fprintf(stderr, "invalid operands to mov on line %d\n", line);
				return -1;
			}
		} else if (!strcmp(op, "in")) {
			if (args != 2) { return -1; }
			if (!strcmp(arg1, "a")) {
				t->rom[s++ % 16] = (0x2 << 4);
			} else if (!strcmp(arg1, "b")) {
				t->rom[s++ % 16] = (0x7 << 4);
			} else {
				return -1;
			}
		} else if (!strcmp(op, "out")) {
			if (args != 2) { return -1; }
			if (!strcmp(arg1, "a")) {
				(void)fprintf(stderr, "'a' is not a valid operand for 'out' on line %d\n", line);
				return -1;
			} else if (!strcmp(arg1, "b")) {
				t->rom[s++ % 16] = (0x9 << 4);
			} else {
				t->rom[s++ % 16] = (0xA << 4) | (atoi(arg1) & 0xF);
			}
		} else if (!strcmp(op, "jc")) { /* labels would be a nice to have feature */
			if (args != 2) { return -1; }
			t->rom[s++ % 16] = (0xE << 4) | (atoi(arg1) & 0xF);
		} else if (!strcmp(op, "jmp")) {
			if (args != 2) { return -1; }
			t->rom[s++ % 16] = (0xF << 4) | (atoi(arg1) & 0xF);
		} else {
			(void)fprintf(err, "invalid operation on line %d: '%s'\n", line, buf);
			return -1;
		}
		if (s > 16) {
			(void)fprintf(err, "too many instructions (>16) on line %d\n", line);
			return -1;
		}
	}
	return 0;
}

static FILE *fopen_or_die(const char *file, const char *mode) {
	assert(file);
	assert(mode);
	const int e = errno;
	errno = 0;
	FILE *f = fopen(file, mode);
	if (!f) {
		(void)fprintf(stderr, "Unable to open '%s' in mode '%s': %s\n", file, mode, strerror(errno));
		exit(1);
	}
	errno = e;
	return f;
}

int main(int argc, char **argv) {
	td4_t td4 = { .pc = 0, }, *t = &td4;
	if (argc < 3) {
		(void)fprintf(stderr, "Usage: %s hex|asm|dump td4.{hex,asm} #steps?\n", argv[0]);
		return 1;
	}
	if (!strcmp(argv[1], "hex")) {
		FILE *in = fopen_or_die(argv[2], "rb");
		for (size_t i = 0; i < 16; i++) {
			unsigned d = 0;
			if (fscanf(in, "%x,", &d) != 1)
				break;
			t->rom[i] = d;
		}
		if (fclose(in) < 0) return 1;
	} else if (!strcmp(argv[1], "asm") || !strcmp(argv[1], "dump")) {
		FILE *in = fopen_or_die(argv[2], "rb");
		if (td4_asm(t, in, stderr) < 0) {
			(void)fclose(in);
			return 1;
		}
		if (fclose(in) < 0) return 1;
		if (!strcmp(argv[1], "dump")) {
			for (size_t i = 0; i < 16; i++) {
				if (fprintf(stdout, "0x%X,\n", t->rom[i]) < 0)
					return -1;
			}
			return 0;
		}
	} else {
		(void)fprintf(stderr, "not implemented '%s'\n", argv[1]);
		return 1;
	}
	const int steps = argc > 3 ? atoi(argv[3]) : 0;
	if (steps < 0) return 1;
	for (int s = 0; !steps || s < steps; s++)
		if (td4_step(t, stdin, stdout) < 0) break;
	return 0;
}

