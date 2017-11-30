#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <bcm2835.h>



int main(int argc, char ** argv) {
    //int flag = 1;
    //int brojac = 0;  
    char vrednost [8];

    int PM25 = 0;
	int PM10 = 0;
    
    //char odDo [32];
    //-------------------------
	//----- SETUP USART 0 -----
	//-------------------------
	//At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively
	int uart0_filestream = -1;
	
	

    
while(1)
{
//OPEN THE UART
	//The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//		O_RDONLY - Open for reading only.
	//		O_RDWR - Open for reading and writing.
	//		O_WRONLY - Open for writing only.
	//
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//											immediately with a failure status if the output can't be written immediately.
	//
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
	uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
	}
	
	//CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
    
    fcntl(uart0_filestream, F_SETFL, 0);



//----- CHECK FOR ANY RX BYTES -----
	if (uart0_filestream != -1)
	{
		// Read up to 255 characters from the port if they are there
		unsigned char rx_buffer[256];
		int rx_length = read(uart0_filestream, (void*)rx_buffer, 255);		//Filestream, buffer to store in, number of bytes to read (max)
		if (rx_length < 0)
		{
			//An error occured (will occur if there are no bytes)
		}
		else if (rx_length == 0)
		{
			//No data waiting
		}
		else
		{         
            
            if(rx_buffer[0] == 0xaa && rx_buffer[1] == 0xc0 && rx_buffer[6] == 0xb0 && rx_buffer[7] == 0x30 && rx_length == 8){
                for (int i=0;i<rx_length;i++){
                    printf("  %x", rx_buffer[i]);
                    vrednost[i] = rx_buffer[i];
                }  
                //UPISIVANJE P VREDNOSTI
                        printf("\n");
                        
                        PM25= ((vrednost[3] *256) + vrednost[2])/10; 
                        printf("  PM2,5 : %d ", PM25);
                        printf("[%x ", vrednost[3]);
                        printf(", %x", vrednost[2]);
                        printf("]\n");
                        
                        PM10= ((vrednost[5] *256) + vrednost[4])/10; 
                        printf("  PM10  : %d ", PM10);
                        printf("[%x ", vrednost[5]);
                        printf(", %x", vrednost[4]);
                        printf("]\n");
                        

                        //UPISIVANJE P VREDNOSTI                        
                        printf("\n");
            printf("\n");
            }
            
            bcm2835_delay(1000);
		}
	}
    //----- CLOSE THE UART -----
	close(uart0_filestream);
    
  }
  return 0;
}
