

#include "graphics.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	int i;

	init_graphics();

	//Construct an offscreen buffer to draw to
	void *buf = new_offscreen_buffer();

	char key;
	int n = 1;
    
	
    do{
        draw_line(buf, 50,50,100,50,RGB(30,30,30));
        draw_line(buf, 50,100,100,100,RGB(30,30,30));
        draw_line(buf, 50,50,50,100,RGB(30,30,30));
        draw_line(buf, 100,50,100,100,RGB(30,30,30));
        blit(buf);

        draw_text(buf, 300,300, "PRESS Q TO SEE NEXT DEMO", RGB(40,30,30));
        blit(buf);

        key = getkey();
        if (key == 'q')
        {
            break;
        }
        sleep_ms(200);

    }while (1);
   
    clear_screen(buf);


    do{
        draw_text(buf, 50, 50,"CS1550 PROJECT 1", RGB(20,5,10));
        blit(buf);
        draw_text(buf, 300,300, "PRESS Q TO SEE NEXT DEMO", RGB(40,30,30));
        blit(buf);
        key = getkey();
        if(key == 'q')
        {
            break;
        } 
        
        sleep_ms(200);
    }
    while(1);

    for( i = 0; i<480;i++)
    {
        

        draw_pixel(buf,i, 50, RGB(20,5,10));
        draw_pixel(buf,i, 60, RGB(20,5,10));
        draw_pixel(buf,i, 70, RGB(20,5,10));
        blit(buf);
        
        sleep_ms(200);
    }

    
	exit_graphics();
	return 0;

}


