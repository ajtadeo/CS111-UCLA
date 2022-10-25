/////////////////////
// Bootloader Code //
/////////////////////
enum
{
    NEXTPROG_ADDR = 0x1000
};
static char *nextprog_addr = (char *)NEXTPROG_ADDR;
static void (*nextprog)(void) = (void (*)(void))NEXTPROG_ADDR;

// inb()
// reads to 8 bits from a given I/O register
static unsigned char inb(unsigned short port)
{
    unsigned char data;
    asm volatile("inb %0, %1" : "=a"(data) : "dN"(port));
    return data;
}

// outb()
// copies data from flash into flash RAM controller registers
static void outb(unsigned short port, unsigned char data)
{
    asm volatile("outb %0, %1": : "dN"(port), "a"(data));
}

// insl()
// reads words of a specific register in flash RAM and catch it in CPU RAM
// port = device register to grab from
// addr = memory location we want to store sequentially into
// count = number of words
static void insl(unsigned long port, void *addr, int count)
{
    asm volatile("cld;"
                 "repne; insl;"
                 : "=D"(addr), "=c"(count)
                 : "d"(port), "0"(addr), "1"(count)
                 : "memory", "cc");
}

// read_ide_sector()
// reads IDE sector of a flash drive and stores that data in CPU RAM
static void read_ide_sector(long secno, char *addr)
{
    while ((inb(0x1f7) & 0xc0) != 0x40)
        continue;
    outb(0x1f2, 1); // sector count
    outb(0x1f3, secno);
    outb(0x1f4, secno >> 8);
    outb(0x1f5, secno >> 16);
    outb(0x1f6, secno >> 24);
    outb(0x1f7, 0x20); // status and control register, sends "READ SECTORS" cmd
    // masks out the first 2 bits from the 8 bit register: “01” pattern signifies flash drive is READY
    while ((inb(0x1f7) & 0xc0) != 0x40)
        continue;
    insl(0x1f0, addr, 128); // read 128 words (512 bytes)
}

// bootloader()
// takes 20 sectors from flash drive containing the file’s word count and inserts that data into a position in RAM
void bootloader(void)
{
    for (int i = 1; i <= 20; i++)
        read_ide_sector(i, nextprog_addr + (i - 1) * 512);
    nextprog();
}

// write_integer_to_console()
// converts the values in RAM into ascii characters using memory-mapped I/O
void write_integer_to_console(long n)
{
    unsigned short *p = (unsigned short *)0xb8014;
    while (n)
    {
        *p-- = 7 << 8 | ('0' + n % 10);
        n /= 10;
    }
}

///////////////
// Main Code //
///////////////
void main (void)
{
	long words = 0;
	bool in_word = false;
	long secno = 100000;
	for (;;secno++)
	{
		char buf[512];
		read_ide_sector(secno, buf);
		for (int j=0; j < 512; j++){
			// if we find the null byte then we end the program
			if (!buf[j]){
				write_integer_to_console(words + in_word);
				while (true) continue; // no where to return to :(
			}
			bool this_alpha = isalpha((unsigned char) buf[j]);
			words += in_word & ~this_alpha; // we found a word!
			in_word = this_alpha;
		}
}