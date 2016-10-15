void num2str(unsigned char *buf, unsigned char number){
    unsigned char i, remainder, length = 0, n;
 
    n = number;
	
	// find number of digits
    while (n != 0)
    {
        length++;
        n /= 10;
    }
	
	// store each digit as a character in the buffer
    for (i = 0; i < length; i++)
    {
        remainder = number % 10;
        number = number / 10;
        buf[length - (i + 1)] = remainder + '0';
    }
	
    buf[length] = '\0'; // append a null character
}
