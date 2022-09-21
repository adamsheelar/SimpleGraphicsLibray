#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "graphics.h"
#include "iso_font.h"


int fileDescriptor, total_size;
color_t *buffer;
struct fb_var_screeninfo varScreenInfo;
struct fb_fix_screeninfo fixScreenInfo;
struct termios settings;

void init_graphics()
{
    fileDescriptor = open("/dev/fb0", O_RDWR);

    ioctl(fileDescriptor, FBIOGET_VSCREENINFO,&varScreenInfo);
    ioctl(fileDescriptor, FBIOGET_FSCREENINFO,&fixScreenInfo);
    total_size = varScreenInfo.yres_virtual * fixScreenInfo.line_length;
    
    buffer = (color_t*)mmap(NULL, total_size, PROT_READ|PROT_WRITE, MAP_SHARED, fileDescriptor, 0);
    //note \033[2J is a sequence that clears the screen, learned via google 
    
    write(0, "\033[2J", 8);

    // Disable Keypress Echo
    ioctl(0,TCGETS,&settings);
    settings.c_lflag &= ~ICANON;
    settings.c_lflag &= ~ECHO;
    ioctl(0,TCSETS,&settings);
}

void exit_graphics()
{

  write(0, "\033[2J", 8);

  ioctl(0,TCGETS,&settings);
  settings.c_lflag |= ECHO;
  settings.c_lflag |= ICANON;
  ioctl(0, TCSETS, &settings);
  
  munmap(buffer,total_size);
  close(fileDescriptor);
}


char getkey()
{
  fd_set rfds;
  struct timeval tv;
  
  FD_ZERO(&rfds);
  FD_SET(0,&rfds);

  // Waiting only 1 second instead of 5
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  char key = '\0';
  if(select(1, &rfds, NULL, NULL, NULL))
  {
    read(0,&key,1);
  }
  return key;
}
void sleep_ms(long ms)
{

  if(ms > 0)
  {
    struct timespec tv;
    // tv_nsec has a max value, so split into tv_nsec and tv_sec

    tv.tv_sec = ms / 1000;
    tv.tv_nsec = (ms%1000) * 1000000;
    nanosleep(&tv, NULL);
  }
}

void clear_screen(void *img)
{
    //create a new screen buff so we dont derefrence a void 
    color_t *offset = (color_t*) img;
    int x;

    //set all pixels to 0 which clears the screen 
    for(x=0;x<total_size/2;x++)
    {
        buffer[x] = 0;
        offset[x] = 0;
    }
}

void draw_pixel(void *img, int x, int y, color_t color)
{
  // please dont go out of bounds that would be nice

  if(x<0||y<0)
  {
    return;
  }
  if(x>=varScreenInfo.xres_virtual||y>=varScreenInfo.yres_virtual)
  {
    return;
  }
  color_t *offset = (color_t*) img;
  //  pixel at (x,y) to be given color, index is x + y * (number of pixels in x direction)
  offset[x+(y * varScreenInfo.xres_virtual)] = color;
}

// Rosetta Code
//https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
void draw_line(void *img, int x1, int y1, int x2, int y2, color_t c)
{
  int dx, dy, sx, sy, err, e2;
  if(x1 < x2)
  {
    dx = x2 - x1;
    sx = 1;
  }
  else
  {
    dx = x1 - x2;
    sx = -1;
  }
  if(y1 < y2)
  {
    dy = y2 - y1;
    sy = 1;
  }
  else
  {
    dy = y1 - y2;
    sy = -1;
  }
  if(dx > dy)
  {
    err = dx/2;
  }
  else err = -dy/2;
  for(;;)
  {
    draw_pixel(img, x1, y1, c);
    if (x1==x2 && y1==y2)
      break;
    e2 = err;
    if (e2 >-dx)
    {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dy)
    {
      err += dx;
      y1 += sy;
    }
  }
}

void *new_offscreen_buffer()
{
  return mmap(NULL, total_size, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void blit(void *src)
{
  color_t *offset = (color_t *)src;
  int x;
  for(x = 0; x < total_size/2; x++)
  {
    buffer[x] = offset[x];
  }
}

void draw_text(void *img, int x, int y, const char *text, color_t  c) 
{
    int n=0;
    char t = text[n];
    while(t!='\0'){ /* iterate over the whole string */

    int i;
    //loop to end of the x length 
    for(i=0; i<16; i++){
        unsigned char byte = iso_font[ 16 * text[n] + i];
        int j;
        int k=0;
        //128 --> 8*16
        for(j=1; j<=128; j*=2){
            //mask it make sure we have a valid letter 
            if(byte&j)
                draw_pixel(img, y+k, x+i, c);
            //had to swap x, and y here becuase if i put x first the letters printed verticly and not horizontaly
            k++;
        }
    }
    t = text[++n];
    y+=8;
    }

  
}
